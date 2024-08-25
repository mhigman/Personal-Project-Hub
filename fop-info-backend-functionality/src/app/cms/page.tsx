"use client";
import Button from '@/components/Button';
import Container from '@/components/Container';
import { ReactNode, useEffect, useState } from "react";
const Field = ({children, label, id} : { children: ReactNode, label: string, id: string }) => {
    return (
        <label>
            <label htmlFor={id}>
                {label}
            </label>

            {children}
        </label>
    )
}

export default function CMSIndex()
{
    async function imageToServer(data: FormData) {
        console.log(events);
        await fetch('/api/content_management', {
            method: 'POST',
            body: data
        });
        const name = (data.get('header') as string).toLowerCase().replace(" ", "-");
        if(!events.includes(name)){
            setEvents(a => [...a,  name]);
        }
    }

    async function deleteEvent(data: FormData) {
        await fetch('/api/get_event', {
            method: 'DELETE',
            body: data
        }).then((res) => res.json())
        .then((data) => {
            console.log(data)
            return data
        });
        setEvents(a => a.filter((val) =>{
            return val !== (data.get('id') as string).toLowerCase().replace(" ", "-");
        }))
    }


    const [events, setEvents] = useState<Array<string>>([]);

    useEffect(() => {
        async function getEvents() {
            setEvents(await fetch("/api/get_event", {
                method: "GET"
            }).then((res) => {
                return res.json()
            }).then((data) => {
                return data.events
            }))
        }
        getEvents();
    },
    [])


    return <Container padded>
        <form action={imageToServer}>

            <Field id='header' label='Title'>
                <input name='header' id='header' required />
            </Field>

            <Field id='body' label='Content'>
                <textarea name='body' id='body' required rows={4} />
            </Field>

            <Field id='attachment' label='Attachment'>
                <input type='file' name='attachment' id='attachment' accept="image/*" />
            </Field>

            <Button variant='cta' size='small' type='submit' style={{ marginBottom: 12 }}>
                Submit
            </Button>

        </form>
        <form action={deleteEvent}>
            <Field id='id' label='Event Name'>
                <select name='id' id='id'>
                    <option value="">--Please choose an option--</option>
                    {events.map((val, index) => {
                        return <option value={val} key={index}>{val}</option>
                    })}
                </select>
            </Field>
            <Button variant='cta' size='small' type='submit' style={{ marginBottom: 12 }}>
                Delete
            </Button>
        </form>

        
    </Container>;
}