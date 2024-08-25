import { validateSession } from "@/util/dbLogic";
import { NextResponse } from "next/server";
import type { NextRequest } from "next/server";
import { Ratelimit } from "@upstash/ratelimit";
import { kv } from "@vercel/kv";
const laxRateLimit = new Ratelimit({
    redis: kv,
    prefix: "ratelimit:lax",
    limiter: Ratelimit.slidingWindow(5, "15 s")

});
const strictRateLimit = new Ratelimit({
    redis: kv,
    prefix: "ratelimit:strict",
    limiter: Ratelimit.slidingWindow(5, "60 s")
})



export async function middleware(req: NextRequest){
    const currentSid = req.cookies.get('sid');
    console.log(currentSid);

    let isValid = true;

    if(!currentSid || !currentSid.value){
        isValid = false;
    } else {
         isValid = await validateSession(currentSid.value);
    }

    console.log(isValid)


   
    


    if((req.nextUrl.pathname.startsWith("/cms") ||
        req.nextUrl.pathname.startsWith("/api/content_management") ||
        req.nextUrl.pathname.startsWith("/api/get_event")) && !isValid){
        return NextResponse.redirect(new URL("/auth/login", req.url))
    }

    if(!req.nextUrl.pathname.startsWith("/api") && !isValid){
        const ip = req.ip ?? '127.0.0.1';
        const { success, pending, limit, reset, remaining } = await laxRateLimit.limit(
            ip
        );
        return success 
            ? NextResponse.next()
            : NextResponse.redirect(new URL("/blocked", req.url))
    } else if(req.nextUrl.pathname.startsWith("/api") && !isValid) {
        const ip = req.ip ?? '127.0.0.1';
        const { success, pending, limit, reset, remaining } = await strictRateLimit.limit(
            ip
        );
        return success 
            ? NextResponse.next()
            : NextResponse.redirect(new URL("/blocked", req.url))
    }
  
}

export const config = {
    matcher: ['/((?!_next/static|_next/image|favicon.ico).*)']
}