import type { UserType } from "@/types/UserType";
import type { Authenticator } from "@/types/Authenticator";
import uint8ToBase64, { base64ToUint8 } from "@/util/uint8ToBase64";
import { SerializedAuthenticator } from "@/types/SerializedAuthenicator";
import { sha512 } from "./sha512";
import { createClient } from "@vercel/postgres";
import type { CredentialDeviceType } from "@simplewebauthn/types";
import crypto from "crypto";

//TODO: Migrate this into a db. Since I cant even user
// read/write file, I have to use a global json state.


// A function to generate 16 cryptographically random bytes to salt a hash
function generateHashingSalt() {
    return crypto.randomBytes(16).toString("hex");
}

// A function to get a specific user from the database using their email.
export async function getUserFromDB(email: string){
    const client = createClient();

    let user: UserType = {associationID: 0, id: "", email: ""};



    try {
        await client.connect();

        const { rows, fields } = 
            await client.sql`SELECT * FROM authusers WHERE email = ${email}`

        if(!(rows === null || rows.length < 1)){
            user = {
                associationID: parseInt(rows[0].associationid),
                id: rows[0].id,
                email: rows[0].email,
                currentChallenge: rows[0].currentchallenge
            }
        }
    } catch {

    } finally {
        await client.end()
    }

    return user
}

// A function to get all previous authenticators, given a user's associationID 
// (the unix timestamp of account creation)
export async function getPreviousAuthenticators(associationID: number){
    
    const client = createClient();

    let authenticators: SerializedAuthenticator[] = [];

    try {
        await client.connect();

        const { rows, fields } = 
            await client.sql`SELECT * FROM authenticators WHERE associationid = ${associationID}`

        if(!(rows === null)){
            authenticators = rows.map((value) => {

                return {
                    associationID: parseInt(value.associationid),
                    credentialID: (value.credentialid as string),
                    credentialPublicKey: (value.credentialpublickey as string),
                    counter: parseInt(value.counter),
                    credentialDeviceType: (value.credentialdevicetype as CredentialDeviceType),
                    credentialBackedUp: parseInt(value.credentialbackedup) == 0 ? false : true,
                    transports: (value.transports as string).split(" ") as AuthenticatorTransport[]
                }
            });
        }
    } catch {

    } finally {
        await client.end()
    }

    return authenticators;

}



// A function that adds a user into the database given certain values.
export async function addUserIntoDB(user: UserType){
    const client = createClient();
    let error = false;
    try {
        await client.connect();
        if(!user.currentChallenge){
            await client.sql`INSERT INTO authusers (associationID, id, email, currentChallenge) VALUES (${user.associationID}, ${user.id} ${user.email}, None)`
        }
        await client.sql`INSERT INTO authusers (associationID, id, email, currentChallenge) VALUES (${user.associationID}, ${user.id}, ${user.email}, ${user.currentChallenge})`
    } catch {
        error = true
    } finally {
        await client.end();
    }
    return error
}


// A function to get a new callenge 
// (a randomly generated sequence of bytes represented by a hex string)
export async function updateChallenge(challenge: string, associationID: number){
    const client = createClient();
    let error = false;
    try {
        await client.connect();
       
        await client.sql`UPDATE authusers SET currentChallenge = ${challenge} WHERE associationID = ${associationID}`
    } catch {
        error = true
    } finally {
        await client.end();
    }
    return error
}


// A function to add an authenticator object to the database given certain values
export async function addAuthenticator(newAuthenticator: Authenticator){
    //convert some stuff to b64 using the util thing
    const parsedAuthenticator = {
        associationID: newAuthenticator.associationID,
        credentialID: uint8ToBase64(newAuthenticator.credentialID),
        counter: newAuthenticator.counter,
        credentialPublicKey: uint8ToBase64(newAuthenticator.credentialPublicKey),
        credentialBackedUp: newAuthenticator.credentialBackedUp,
        credentialDeviceType: newAuthenticator.credentialDeviceType,
        transports: newAuthenticator.transports?.join(" ")
    }


    const client = createClient();

    let error = false

    try {
        await client.connect()

        console.log(parsedAuthenticator)
        
        await client.sql`INSERT INTO authenticators (associationID, credentialID, counter, credentialPublicKey, credentialBackedUp, credentialDeviceType, transports) VALUES (${parsedAuthenticator.associationID}, ${parsedAuthenticator.credentialID}, ${parsedAuthenticator.counter}, ${parsedAuthenticator.credentialPublicKey}, ${parsedAuthenticator.credentialBackedUp ? 1 : 0}, ${parsedAuthenticator.credentialDeviceType}, ${parsedAuthenticator.transports})`

    } catch (e) {
        error = true;
        console.log("error");
        console.log(e);
    } finally {
        await client.end();
    }

    console.log(error);

    return error
}


// A function to update the "counter" field of an authernticator, identified by its associationID,
// in the database
export async function saveUpdatedAuthenticatorCounter(associationID: number, newCounter: number){
    const client = createClient();
    let error = false;
    try {
        await client.connect();
        
        await client.sql`UPDATE authenticators SET counter = ${newCounter} WHERE associationID = ${associationID}`
    } catch {
        error = true
    } finally {
        await client.end();
    }
    return error
}


// A simple helper function to help determing the amount of padding
// characters in a base64 url encoded string with a certain length.
function findPaddingChars(length: number) {
    switch (length % 4) {
        case 0:

            return 0;
        case 2:

            return 2;
        case 3:

            return 1;
        default:

            return 0;
    }
}

// A function to get a certain authenticator in the database given
// its id and associationID
export async function getAuthenticator(id:string, associationID: number){


    const authenticators = await getPreviousAuthenticators(associationID);
    for(let value of authenticators){
        const idnotURL = (id + 
            "=".repeat((findPaddingChars(id.length))))
            .replace(/-/g, "+")
            .replace(/_/g, "/")
        console.log(idnotURL)
        console.log(value.credentialID)
        if(value.credentialID === idnotURL){
            const regularAuthenticator = {
                associationID: value.associationID,
                credentialID: base64ToUint8(value.credentialID),
                credentialPublicKey: base64ToUint8(value.credentialPublicKey),
                counter: value.counter,
                credentialBackedUp: value.credentialBackedUp,
                credentialDeviceType: value.credentialDeviceType,
                transports: value.transports
            }
        
            return regularAuthenticator;
        }
    }
}



// A function to add a sessionID token with a 5 minute
// expire time
export async function addSession(sessionID: string) {


    const client = createClient();
    await client.connect()

    const sidAndId = sessionID.split(";")

    const salt = generateHashingSalt()
    const hashedsessionID:string = await sha512(sidAndId[0], salt)
    const expiretime: number = Date.now() + 300000

    console.log()
    console.log(expiretime)
    try{
        const insertion = await client.sql`INSERT INTO sessions (sessionid, expiretime, salt, id) VALUES (${hashedsessionID}, ${expiretime}, ${salt}, ${sidAndId[1]})`
    } finally {
        await client.end()
    }


    return
}

// A function to validate a session given a sessionID and UUID (or equivalent)
// delineated by a semicolon.
export async function validateSession(sessionID: string) {
    
    
    const client = createClient()
    await client.connect()

    const sidAndId = sessionID.split(";")



    let valid = true;
    try{

        await client.sql`DELETE FROM sessions WHERE expiretime<${Date.now()}`

        const { rows, fields } = 
            await client.sql`SELECT * FROM sessions WHERE id = ${sidAndId[1]}`;


            

        console.log(rows)

        if(rows === null || rows.length < 1){
            valid = false
        }

        if(rows[0].sessionid === null) {
            valid = false
        }


        const salt = rows[0].salt
        const hashedsessionID = await sha512(sidAndId[0], salt)

        if(rows[0].sessionid !== hashedsessionID){
            valid = false;
        }


       
    } catch {
        valid =  false;
    } finally {
        await client.end()
    }
    

    return valid


}