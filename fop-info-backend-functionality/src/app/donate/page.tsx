
"use client";

import { redirect } from "next/navigation";
import Button from "@/components/Button";
import Container from "@/components/Container";
import { useState } from "react";


export default function DonationPage() {
    const [error, setError] = useState(false);


    async function redirectToStripe(data: FormData) {
        setError(false);
        
        if(!data.get('amount')){
            console.log("getting data no")
            setError(true);
            return;
        } 
        //@ts-ignore
        const amountGet: FormDataEntryValue = data.get('amount') as string;

        const amount: number = parseInt(amountGet) * 100

       

        const session = await fetch('/api/donate', {
            method: "POST",
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                amount: amount
            })
        }).then((res)=>{
            setError(!res.ok)
            return res.json()
        }).then((data)=> {
            setError(error && !data.url)
            return data
        })

        
        if(!error){
            console.log(session)
            redirect(session.url)
        }
    }


    return <Container padded>          
        <h1>Thank You for donating!</h1>
        <label>{error && "Something went wrong"}</label>
        <form action={redirectToStripe}>

            <input type="number" name='amount' step={0.01}/>
            
            <Button type="submit">
                Donate
            </Button>
        </form>
    </Container>;
}