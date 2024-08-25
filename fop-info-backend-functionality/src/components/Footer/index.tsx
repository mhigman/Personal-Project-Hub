"use client";
import type { FC, ReactNode } from 'react'
import type { ContactFormData } from '@/types/ContactFormData'
import formDataToJSON from '@/util/formDataToJSON'
import ContactTemplate from '../Resend/ContactTemplate'
import style from './style.module.scss'
import Container from '../Container'
import Button from '../Button'
import { useState } from 'react'

const Field = ({children, label, id} : { children: ReactNode, label: string, id: string }) => {
    return (
        <label className={style.input}>
            <label htmlFor={id}>
                {label}
            </label>

            {children}
        </label>
    )
}



const Footer: FC = () => {
    const [error, setError] = useState(0);
    async function submitAction(data: FormData) {
        const dataJson: ContactFormData = JSON.parse(formDataToJSON(data))
        const response = await fetch('api/contact_form', {
            method: "POST",
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(dataJson)
        }).then((res)=> {
            setError(res.ok ? 1 : 2)
            return res.json()
        })

        return
    }
    return (
        <Container padded style={{ display: 'flex', gap: 48, marginBottom: 24 }}>
            <div style={{ flex : 5 }}>
                <h2>Benton County Fraternal Order of Police</h2>

                <p>We are a non-profit organization that still requires a disclaimer to be sent over.</p>
            
                <p>Send us mail:<br/>PO 124365, Centerton, AR. 72719</p>

                <Button variant='cta'>
                    Donate
                </Button>

                <p style={{ marginTop: '2em' }}>Visit our socials:</p>
            </div>
            <div style={{ flex : 4 }}>

                <h2>Contact</h2>
                {error == 2 && <p>Something went wrong</p>}
                {error == 1 && <p>Email Successfully Sent</p>}
            <form action={submitAction}>
                <Field label='Name' id='name'>
                    <input type='text' autoComplete='first_name' name='name' required />
                </Field>

                <Field label='Email' id='email'>
                    <input type='email' autoComplete='email' name='email' required />
                </Field>

                <Field label='Message' id='content'>
                    <textarea name='content' required minLength={5} />
                </Field>

                <Button type='submit' variant='cta'>
                    Submit
                </Button>
            </form>
            </div>
        </Container>
    );
}

export default Footer
