"use client";

import { ReactNode, useEffect, useMemo, useState } from "react"
import styles from "./style.module.scss"
import Container from "@/components/Container"
import Flex from "@/components/Flex"
import Button from "@/components/Button"
import { deserializeCredentials, serializeCredentials } from "@/util/credentialSerializer";
import { startRegistration, startAuthentication } from "@simplewebauthn/browser";
import { useRouter } from "next/navigation";


export default function Login() {
    let successLogin = false;
    const [sucessRegister, setSuccessRegister] = useState(false);

    const { push } = useRouter()

    const register = async () => {

        const resp = await fetch('/api/auth/generate-registration-options', {
            method: "POST",
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                email: "admin@fop.wwise.dev"
            })
        }).then((res) => {
            const resJson = res.json()
            console.log( resJson )
            return resJson
        });
        console.log(resp)
        let attResp;
        try {
          // Pass the options to the authenticator and wait for a response
          attResp = await startRegistration(resp);
        } catch (error: any) {
          // Some basic error handling            
          //Probably show UI message or something

          if (error.name === 'InvalidStateError') {
            //'Error: Authenticator was probably already registered by user';
          } else {
            //error;
          }
    
          throw error;
        }
        const verificationResponse = await fetch("/api/auth/verify-registration", {
            method: "POST",
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                email: "admin@fop.wwise.dev",
                response: attResp
            }),
        })
        .then((res) => {
            setSuccessRegister(res.ok)
            return res.json()
        });

        console.log(sucessRegister);
        console.log(verificationResponse);
        if(sucessRegister){
            push("/")
        }
    }   

    const login = async () => {
        const resp = await fetch('/api/auth/generate-authentication-options', {
            method: "POST",
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                email: "admin@fop.wwise.dev"
            })
        }).then((res) => {
            const resJson = res.json()
            console.log( resJson )
            return resJson
        });


        let attResp;
        try {
          // Pass the options to the authenticator and wait for a response
          attResp = await startAuthentication(resp);
        } catch (error: any) {
          // Some basic error handling            
          //Probably show UI message or something

          if (error.name === 'InvalidStateError') {
            //'Error: Authenticator was probably already registered by user';
          } else {
            //error;
          }
    
          throw error;
        }
        const verificationResponse = await fetch("/api/auth/verify-authentication", {
            method: "POST",
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                email: "admin@fop.wwise.dev",
                response: attResp
            }),
        })
        .then((res) => {
            console.log("done")
            
            successLogin = res.ok
            return res.json()
        });


        if(successLogin){
            push("/")
        }
    }

    return (
        <main className={styles.main}>
            <div className={styles.inner}>
                <Flex alignItems="center" flexDirection="column">
                    <h1 style={{ marginBottom: 0 }}>Administration Login</h1>

                    <div style={{ width: 360, textAlign: 'center' }}>
                        <p style={{ margin: '20px 0' }}>
                            This page is intended for administrators.<br/><a href="/">Go back</a>
                        </p>

                        <Button variant='cta' size='small' style={{ width: '100%' }} onClick={login}>
                            Sign in with a passkey
                        </Button>

                        <Button size='small' style={{ width: '100%', marginTop: 8 }} onClick={register}>
                            Test register
                        </Button>
                    </div>
                </Flex>
            </div>
        </main>
    )
}
