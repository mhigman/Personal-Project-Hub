import { verifyAuthenticationResponse } from "@simplewebauthn/server";
import { NextRequest, NextResponse } from "next/server";
import { readFile, writeFile } from "fs/promises";
import type { UserType } from "@/types/UserType";
import { Authenticator } from "@/types/Authenticator";
import uint8ToBase64, { base64ToUint8 } from "@/util/uint8ToBase64";
import { getUserFromDB, saveUpdatedAuthenticatorCounter, getAuthenticator, addSession } from "@/util/dbLogic";
import crypto from "crypto";
function getSessionID() {
    return crypto.randomBytes(64).toString("hex") + ";" +  crypto.randomBytes(32).toString("hex");
}


export async function POST(req: NextRequest) {
    const data = await req.json();
    const email: string = data.email;
    const rpName = "FOP Login Portal";
    const rpID = process.env.hostname;
    const origin = process.env.origin;


    if(!origin){
        return NextResponse.json({
            message: "Something went wrong"
        }, {
            status: 500
        });
    }



    const currentUser: UserType = await getUserFromDB(email);


    const authenticator: Authenticator | undefined = await getAuthenticator(data.response.id, currentUser.associationID);


    if(!authenticator) { 
        return NextResponse.json({
            message: `Couldn't find authenticator ${data.response.id} for ${email}`
        }, {
            status: 500
        })
    }




    if(rpID === undefined){
        return NextResponse.json({
            
            message: "Something went wrong in defining hostname."
        }, {  
            status: 500,
            
        })
    }

    const expectedChallenge = currentUser.currentChallenge

    if(!expectedChallenge){
        return NextResponse.json({
            message: "You need a challenge to verify authentication."
        }, {
            status: 403,
        })
    }

    let verification;
    try {
        verification = await verifyAuthenticationResponse({
            response: data.response,
            expectedChallenge,
            expectedOrigin: origin,
            expectedRPID: rpID,
            authenticator,
        });
    } catch (error){
        console.error(error);
        return NextResponse.json({
            message: "Malformed verification data"
        }, {
            status: 400,
        });
    }


    const { verified } = verification;


    if(verified) {
        const { authenticationInfo } = verification;
        const { newCounter } = authenticationInfo;

        if(await saveUpdatedAuthenticatorCounter(authenticator.associationID, newCounter)){
            return NextResponse.json({}, {status: 500})
        }




        const res =  NextResponse.json({
            message: "auth does yipeeee"
        }, {
            status: 200,
        })

        const sid = getSessionID();
        await addSession(sid);

        res.cookies.set("sid", sid);
        return res
    } else {
        return NextResponse.json({
            message: "Not Authorized"
        }, {
            status: 403
        });
    }


}