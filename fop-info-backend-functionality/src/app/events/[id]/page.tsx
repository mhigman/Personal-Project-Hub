import Button from "@/components/Button";
import Container from "@/components/Container";
import Flex from "@/components/Flex";
import Image from "next/image";
export default async function Event({ params }: {params : {id: string}}) {
    const res = await fetch(process.env.origin + '/api/get_event', {
        next: {
            tags: ["events"]
        },
        method: "POST",
        body: JSON.stringify({
            id: params.id
        }),
    }).then((res) => {
        const resjson = res.json()
        return resjson
    }).then((data) => {
        console.log(data)
        return data
    });
    if(res && res.eventname !== "Event Not Found" && res.blobid !== null){
        return (
    <main style={{ display: 'flex', flexDirection: 'column', gap: 24, textAlign: 'center' }}>
        <Container padded>

            <h1>{res.header}</h1>
            

            <Flex gap={48}>
                <div style={{ flex: 2 }}>
                    <p style={{ textAlign: 'left' }}>
                        {res.body}
                    </p>

                    <Button variant="cta">
                        Sponsor this event
                    </Button>
                </div>
                <div style={{ flex: 1 }}>
                    <div style={{ height: 200, backgroundColor: '#ddd', padding: 20}}>
                        <Image 
                            priority
                            key={res.eventname} 
                            src={res.blobid} 
                            alt="" 
                            width={100}
                            height={100}
                            sizes="100vw"
                            style={{width: '100%', height: '100%'}}
                            />
                    </div>
                </div>
            </Flex>
            <div style={{ height: 24 }} />

            <Flex gap={24}>
                <div style={{ flex: 1 }}>
                    <div style={{ height: 200, backgroundColor: '#ddd' }} />
                </div>

                <div style={{ flex: 1 }}>
                    <div style={{ height: 200, backgroundColor: '#ddd' }} />
                </div>

                <div style={{ flex: 1 }}>
                    <div style={{ height: 200, backgroundColor: '#ddd' }} />
                </div>
            </Flex>
        </Container>

        <Container padded>
            <h2>
                Presenting Sponsors
            </h2>
        </Container>

        <Container padded>
            <h2>
                Deputy Chief Sponsors
            </h2>
        </Container>

        <Container padded>
            <h2>
                Major Sponsors
            </h2>

            <ul style={{ listStyleType: 'none', padding: 0 }}>
                {[
                    "Akins Ford",
                    "Delta Air Lines",
                    "Genuine Parts Company",
                    "Holder Construction",
                    "Invesco",
                    "King & Spalding"
                ].map(x => (
                    <li key={x}>{x}</li>
                ))}
            </ul>
        </Container>

        <Container padded>
            <h2>
                Minor Sponsors
            </h2>

            <Flex justifyContent='center' gap={24}>
                {[0, 1].map(i => 
                    <div key={i}>
                        <ul style={{ listStyleType: 'none', padding: 0 }}>
                            {[
                                "Acuity Brands",
                                "Atlanta Fine Homes Sotheby’s International Realty",
                                "Bloom Parham",
                                "Chick-fil-A",
                                "Cox Enterprises",
                                "David & Jennifer Kahn Family Foundation Inc.",
                                "Empire Communities",
                                "Gary J. Pollack",
                                "Georgia Power",
                                "Georgia-Pacific",
                                "Jeff Maggs",
                                "Noel & Somer Hardin",
                                "Piedmont Healthcare",
                                "Rollins",
                                "Synovus",
                                "The Loudermilk Companies",
                                "Waffle House"
                            ].map(x => (
                                <li key={x}>{x}</li>
                            ))}
                        </ul>
                    </div>
                )}
            </Flex>
        </Container>
    </main>
  )
    } else if (res && res.eventname !== "Event Not Found" && res.blobid === null) {
        return (
        <main>
        <Container padded>

            <h1>{res.header}</h1>
            

            <Flex gap={48}>
                <div style={{ flex: 2 }}>
                    <p style={{ textAlign: 'left' }}>
                        {res.body}
                    </p>

                    <Button variant="cta">
                        Sponsor this event
                    </Button>
                </div>
                <div style={{ flex: 1 }}>
                    <div style={{ height: 200, backgroundColor: '#ddd', padding: 20}}>
                        No Image Provided
                    </div>
                </div>
            </Flex>
            <div style={{ height: 24 }} />

            <Flex gap={24}>
                <div style={{ flex: 1 }}>
                    <div style={{ height: 200, backgroundColor: '#ddd' }} />
                </div>

                <div style={{ flex: 1 }}>
                    <div style={{ height: 200, backgroundColor: '#ddd' }} />
                </div>

                <div style={{ flex: 1 }}>
                    <div style={{ height: 200, backgroundColor: '#ddd' }} />
                </div>
            </Flex>
        </Container>

        <Container padded>
            <h2>
                Presenting Sponsors
            </h2>
        </Container>

        <Container padded>
            <h2>
                Deputy Chief Sponsors
            </h2>
        </Container>

        <Container padded>
            <h2>
                Major Sponsors
            </h2>

            <ul style={{ listStyleType: 'none', padding: 0 }}>
                {[
                    "Akins Ford",
                    "Delta Air Lines",
                    "Genuine Parts Company",
                    "Holder Construction",
                    "Invesco",
                    "King & Spalding"
                ].map(x => (
                    <li key={x}>{x}</li>
                ))}
            </ul>
        </Container>

        <Container padded>
            <h2>
                Minor Sponsors
            </h2>

            <Flex justifyContent='center' gap={24}>
                {[0, 1].map(i => 
                    <div key={i}>
                        <ul style={{ listStyleType: 'none', padding: 0 }}>
                            {[
                                "Acuity Brands",
                                "Atlanta Fine Homes Sotheby’s International Realty",
                                "Bloom Parham",
                                "Chick-fil-A",
                                "Cox Enterprises",
                                "David & Jennifer Kahn Family Foundation Inc.",
                                "Empire Communities",
                                "Gary J. Pollack",
                                "Georgia Power",
                                "Georgia-Pacific",
                                "Jeff Maggs",
                                "Noel & Somer Hardin",
                                "Piedmont Healthcare",
                                "Rollins",
                                "Synovus",
                                "The Loudermilk Companies",
                                "Waffle House"
                            ].map(x => (
                                <li key={x}>{x}</li>
                            ))}
                        </ul>
                    </div>
                )}
            </Flex>
        </Container>
    </main>)
    }
    else {
        return (
        <main style={{ display: 'flex', flexDirection: 'column', gap: 24, textAlign: 'center' }}>
            <Container padded>

                <h1>Event Not Found</h1>
                

                <Flex gap={48}>
                    <div style={{ flex: 2, paddingBottom: 100 }}>
                        <p style={{ textAlign: 'left' }}>
                            Please try using a different URL to find the event you are looking for.
                        </p>

                    </div>
                </Flex>
            </Container>
        </main>)
    }
}
