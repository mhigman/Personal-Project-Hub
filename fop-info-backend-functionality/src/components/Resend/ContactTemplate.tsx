import type { ContactFormData } from "@/types/ContactFormData";
import type { FC } from "react";

const ContactTemplate: FC<ContactFormData> = (data) => {
    return <>
        <p>{data.name} ({data.email}) has sent an email.</p>

        <div style={{ border: 'solid 1px #ccc', padding: 16, borderRadius: 8, whiteSpace: 'pre-wrap' }}>
            {data.content}
        </div>

        <p>
            To reply to {data.name.split(' ')[0]}, click <b>Reply all</b>.
        </p>
    </>
}

export default ContactTemplate
