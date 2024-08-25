import { generateRegistrationOptions } from "@simplewebauthn/server";
import { NextRequest, NextResponse } from "next/server";
import type { UserType } from "@/types/UserType";
import type { Authenticator } from "@/types/Authenticator";
import { readFile, writeFile } from "fs/promises";
import uint8ToBase64, { base64ToUint8 } from "@/util/uint8ToBase64";

import { getPreviousAuthenticators, addUserIntoDB, getUserFromDB, updateChallenge } from "@/util/dbLogic";

function createUser(userEmail: string){


    return {
        associationID: Date.now(),
        id: generateUserID(),
        email: userEmail,
    }
}

function generateUserID(){

    return uint8ToBase64(new Uint8Array(32).map(x => Math.floor(Math.random() * 256)))
}

export async function POST(req: NextRequest){
    const email: string = (await req.json()).email;
    const rpName = "FOP Login Portal";
    const rpID = process.env.hostname;
    console.log(rpID)

    const currentUser = await getUserFromDB(email);

    console.log(currentUser)
    if(currentUser && !(currentUser.associationID == 0)){
        if(rpID === undefined){
            return NextResponse.json({
               
                message: "Something went wrong in defining hostname."
            }, {
                status: 500,
            })
        }
        const options = await generateRegistrationOptions({
            rpName,
            rpID,
            userID: currentUser.id,
            userName: currentUser.email,
            attestationType: 'none',
            authenticatorSelection: {
                residentKey: 'preferred',
                userVerification: 'preferred',
            }
    
        });
    
        currentUser.currentChallenge = options.challenge
    
        console.log(currentUser.currentChallenge)
        if(await updateChallenge(currentUser.currentChallenge, currentUser.associationID)){
            return NextResponse.json({}, {status: 500})
        }
    
    
        return NextResponse.json(options, {
            status:200
        });
    }
    if(rpID === undefined){
        return NextResponse.json({
           
            message: "Something went wrong in defining hostname."
        }, {
            status: 500,
        })
    }


    let user: UserType = createUser(email);

    const options = await generateRegistrationOptions({
        rpName,
        rpID,
        userID: user.id,
        userName: user.email,
        attestationType: 'none',
        authenticatorSelection: {
            residentKey: 'preferred',
            userVerification: 'preferred',
        }

    });

    user.currentChallenge = options.challenge


    if(await addUserIntoDB(user)){
        return NextResponse.json({}, {status: 500})
    }


    return NextResponse.json(options, {
        status:200
    });
}