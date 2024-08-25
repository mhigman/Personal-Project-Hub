import { NextRequest, NextResponse } from "next/server"
import { Resend } from "resend"
import ContactTemplate from "@/components/Resend/ContactTemplate"
import type { ContactFormData } from "@/types/ContactFormData"

const resend = new Resend(process.env.RESEND_API_KEY)



export async function POST(req: NextRequest){
    const dataJson: ContactFormData = await req.json()


    const { data: response, error } = await resend.emails.send({
        from: 'User Contact <user-contact@fop.wwise.dev>',
        to: ['fop@realsgii2.dev', 'higmanm@bentonvillek12.org'],
        reply_to: dataJson.email,

        subject: `Contact Request From ${dataJson.name}`,
        react: ContactTemplate(dataJson) as React.ReactElement,
    });

    if(error){
        return NextResponse.json(error, {
            status:400
        })
    }
    return NextResponse.json(response, {
        status: 200
    })

}