import { NextRequest, NextResponse } from "next/server";
import Stripe from "stripe";

const stripe = new Stripe(process.env.STRIPE_API_KEY ? process.env.STRIPE_API_KEY : "");

export async function POST(req: NextRequest){
    const { amount } = await req.json();


    if (!amount) {
        return NextResponse.json({}, {
            status: 400
        })
    }


    const session = await stripe.checkout.sessions.create({
        line_items: [
            {
                price_data: {
                    currency: 'usd',
                    product_data: {
                        name: 'Donation to FOP',
                    },
                    unit_amount: amount,
                    
                },
                quantity: 1
            },
        ],
        mode: 'payment',
        success_url: process.env.origin + '/donate/success',
        cancel_url: process.env.origin + '/donate/cancel'
    });


    if(!session.url){
        console.log("Url bruh")
        return NextResponse.json({}, {
            status: 500
        });
    }

    return NextResponse.json(session, {
        status: 200
    })
}