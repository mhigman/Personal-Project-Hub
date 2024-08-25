import { validateSession } from "@/util/dbLogic";
import { readFile, writeFile } from "fs/promises";
import { revalidateTag } from "next/cache";
import { NextRequest, NextResponse } from "next/server";
import { put, del } from "@vercel/blob";
import { createClient } from "@vercel/postgres";


export async function POST(req: NextRequest){

    const client = createClient();

    try{
    const data: FormData = await req.formData();
    
    await client.connect();

    let attachment: File;

    if(!data.get('header')){

        return NextResponse.json({message: ""}, {status: 400})
    }
        

    const newFileName = (data.get('header') as string).toLowerCase().replace(" ", "-");


    attachment = data.get('attachment') as File;


    const {rows, fields} = await client.sql`SELECT * FROM events WHERE eventname=${newFileName}`
    const exists = rows.length !=0;
    const prevbloburl = rows[0]?.blobid
    if(attachment.size == 0){
        if(!exists){
            const insertion = await client.sql`INSERT INTO events (eventName, header, body) VALUES (${newFileName}, ${data.get('header') as string}, ${data.get('body') as string})`
        } else {
            const insertion = await client.sql`UPDATE events SET eventName=${newFileName}, header=${data.get('header') as string}, body=${data.get('body') as string} WHERE eventName=${newFileName}`

        }
        
    } else {
        const blob = await put(newFileName, attachment, {access:'public'});


        if(exists){
            try{
                await del(prevbloburl)

            } catch(e){
                console.log(e)
            }

            const insertion = await client.sql`UPDATE events SET eventName=${newFileName}, header=${data.get('header') as string}, body=${data.get('body') as string}, blobID=${blob.url} WHERE eventName=${newFileName}`

        }
        else {
          
            const insertion = await client.sql`INSERT INTO events (eventName, header, body, blobID) VALUES (${newFileName}, ${data.get('header') as string}, ${data.get('body') as string}, ${blob.url})`

        }
    }


    
    } catch(e) {
        console.log(e)
        return NextResponse.json({}, {status: 500})
    } finally {
        await client.end()
    }

    revalidateTag("events");
    return NextResponse.json({}, {status: 200});
}
