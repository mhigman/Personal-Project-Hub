import { generateAuthenticationOptions } from "@simplewebauthn/server";
import { NextRequest, NextResponse } from "next/server";
import type { UserType } from "@/types/UserType";
import type { Authenticator } from "@/types/Authenticator";
import { readFile, writeFile } from "fs/promises";
import uint8ToBase64, { base64ToUint8 } from "@/util/uint8ToBase64";
import { SerializedAuthenticator } from "@/types/SerializedAuthenicator";

import { getUserFromDB, getPreviousAuthenticators, updateChallenge } from "@/util/dbLogic";


export async function POST(req: NextRequest){
    const email: string = (await req.json()).email;
    const rpName = "FOP Login Portal";
    const rpID = process.env.hostname;
    console.log(rpID)

    let currentUser: UserType = await getUserFromDB(email);
    console.log(currentUser)



    if(!currentUser || currentUser?.associationID == 0){

        return NextResponse.json({
          
            message: "User not registered, try registering."
        }, {
            status: 403,
        });
    }
    if(rpID === undefined){
        return NextResponse.json({
           
            message: "Something went wrong in defining hostname."
        }, {
            status: 500,
        })
    }

    const userAuthenticators: SerializedAuthenticator[] = await getPreviousAuthenticators(currentUser.associationID);

    console.log(userAuthenticators)

    const options = await generateAuthenticationOptions({
        rpID,
        // Require users to use a previously-registered authenticator
        allowCredentials: userAuthenticators.map(authenticator => ({
            id: base64ToUint8(authenticator.credentialID),
            type: 'public-key',
            transports: authenticator.transports,
        })),
        userVerification: 'preferred'

    });

    currentUser.currentChallenge = options.challenge


    await updateChallenge(currentUser.currentChallenge, currentUser.associationID);


    return NextResponse.json(options, {
        status:200
    });
}