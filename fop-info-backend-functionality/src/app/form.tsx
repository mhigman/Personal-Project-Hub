"use client";

import Button from "@/components/Button";
import formDataToJSON from "@/util/formDataToJSON";
import { ReactNode } from "react";
import style from "./style.module.scss"

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

const Form = () => {
    const action = async (data: FormData) => {
        data.set('attachmentBlob', Buffer.from(await (data.get('attachment') as File).text()).toString())

        fetch('/api/content_management', {
            method: 'POST',
            body: formDataToJSON(data),
        })
    }

    return <form action={action}>
        <Field id='contentType' label='Content Type'>
            <select name='contentType' id='contentType' required>
                <option value='post'>
                    post
                </option>
            </select>
        </Field>
 
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
}

export default Form;
