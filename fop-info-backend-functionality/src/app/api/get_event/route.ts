import { NextRequest, NextResponse } from "next/server";
import { createClient } from "@vercel/postgres";
import type { EventFormData } from "@/types/EventFormData";
import { del } from "@vercel/blob";
import { validateSession } from "@/util/dbLogic";
import { revalidateTag } from "next/cache";
import { M_PLUS_1 } from "next/font/google";

export async function GET(){
    const client = createClient();
    let rows: Array<string> = []
    try{
        await client.connect();
        const { rows: dbrows, fields } = 
            await client.sql`SELECT * FROM events`
        dbrows.map((value) => {
            rows.push((value as EventFormData).eventname);
            return value;
        })
    } catch(e) {
        console.log(e)
        return NextResponse.json({}, {status: 500})
    } finally {
        await client.end();
    }
    console.log
    return NextResponse.json({events: rows}, {status: 200})
}

export async function POST(req: NextRequest){
    const data = await req.json()
    const client = createClient();
    let event: EventFormData = {
        eventname: 'Event Not Found',
        header: 'Event Not Found',
        body: 'Please try using a different URL to find the event you are looking for.',
        blobid: null,
        
    }

    if(!data || !data.id)
        return NextResponse.json({}, {status: 400})

    try{
        await client.connect();
        const { rows, fields } = 
            await client.sql`SELECT * FROM events WHERE eventName = ${data.id}`
        console.log(rows);
        if(!(rows === null || rows.length < 1)){
            event = rows[0] as EventFormData;
        }
    } catch(e) {
        console.log(e)
        return NextResponse.json({}, {status: 500})
    } finally {
        await client.end();
    }
    return NextResponse.json(event, {status:200})
}


export async function DELETE(req: NextRequest){
    const data = await req.formData()
    const client = createClient();
    if(!data || !data.get('id'))
        return NextResponse.json({}, {status: 400})

    try{
        await client.connect();

        const { rows, fields } = 
            await client.sql`SELECT * FROM events WHERE eventName = ${data.get('id') as string}`

        const deletion = 
            await client.sql`DELETE FROM events WHERE eventName = ${data.get('id') as string}`


        if(!rows|| rows.length < 1) {
            return NextResponse.json({}, {status: 500})
        }

        console.log(rows[0].blobid)
        if(rows[0].blobid !== null){

        
        try{
            await del(rows[0].blobid)
        } catch {
            console.log("no image found")
        }
    }
        console.log("revalidated")
        revalidateTag("events");
        
            
    } catch {
        return NextResponse.json({}, {status: 500})
    } finally {
        await client.end();
    }
    
    return NextResponse.json({}, {status:200})
}