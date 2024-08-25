import { verifyRegistrationResponse } from "@simplewebauthn/server";
import { NextRequest, NextResponse } from "next/server";
import { readFile, writeFile } from "fs/promises";
import type { UserType } from "@/types/UserType";
import { Authenticator } from "@/types/Authenticator";
import uint8ToBase64 from "@/util/uint8ToBase64";
import { getUserFromDB, addAuthenticator } from "@/util/dbLogic";




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

    console.log(currentUser)
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
            message: "You need a challenge to verify registration."
        }, {
            status: 403,
        })
    }

    let verification;
    try {
        verification = await verifyRegistrationResponse({
            response: data.response,
            expectedChallenge,
            expectedOrigin: origin,
            expectedRPID: rpID,
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
        const { registrationInfo } = verification;

        if (!registrationInfo){
            return NextResponse.json({
                message: "Nonexistent User"
            }, {
                status: 403,
            })
        }

        const {
            credentialPublicKey,
            credentialID,
            counter,
            credentialDeviceType,
            credentialBackedUp
        } = registrationInfo

        const newAuthenticator: Authenticator = {
            associationID: currentUser.associationID,
            credentialID,
            credentialPublicKey,
            counter,
            credentialDeviceType,
            credentialBackedUp,
            // `body` here is from Step 2
            transports: data.response.response.transports,
          };
          console.log(newAuthenticator)
          if(!(await addAuthenticator(newAuthenticator))){
            return NextResponse.json({
                message: "resgester does yipeeee"
            }, {
                status: 200
            })
          } else {
            return NextResponse.json({}, {status: 500})
          }

          
    } else {
        return NextResponse.json({
            message: "Not Authorized"
        }, {
            status: 403
        });
    }


}