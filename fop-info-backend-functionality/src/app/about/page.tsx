import Button from "@/components/Button";
import Card from "@/components/Card";
import Container from "@/components/Container";
import Flex from "@/components/Flex";

export default async function Event() {
  return (
    <main style={{ display: 'flex', flexDirection: 'column', gap: 24, textAlign: 'center' }}>
        <Container padded>
            <h1>
                About Us
            </h1>

            <Flex gap={48}>
                <div style={{ flex: 2 }}>
                    <p style={{ textAlign: 'left' }}>
                        The Benton County Fraternal Order of Police is a non-profit organization that aids Centerton’s officers through safety programs and community involvement. Since I don’t have enough information about this organization yet, I’ll just add a lot of random text here to fill up space. 
                        <br/><br/>
                        Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.
                    </p>
                </div>

                <div style={{ flex: 1 }}>
                    <div style={{ height: 200, backgroundColor: '#ddd' }} />
                </div>
            </Flex>
        </Container>

        <Container style={{ borderRadius: 12, backgroundColor: '#eee', padding: 48 }}>
            <h2>
                Our Core Values
            </h2>

            <Flex gap={16} justifyContent="stretch">
                <Card solid style={{ flex: 1 }}>
                    <h3>Core value 1</h3>
                    <p>
                        Hello world, this is a description about this specific principle.
                    </p>
                </Card>
                <Card solid style={{ flex: 1 }}>
                    <h3>Core value 2</h3>
                    <p>
                        Hello world, this is a description about this specific principle.
                    </p>
                </Card>
                <Card solid style={{ flex: 1 }}>
                    <h3>Core value 3</h3>
                    <p>
                        Hello world, this is a description about this specific principle.
                    </p>
                </Card>
                <Card solid style={{ flex: 1 }}>
                    <h3>Core value 4</h3>
                    <p>
                        Hello world, this is a description about this specific principle.
                    </p>
                </Card>
            </Flex>
        </Container>

        <Container style={{ borderRadius: 12, backgroundColor: '#eee', padding: 48 }}>
            <h2>
                Executives
            </h2>

            <Flex gap={16} justifyContent="stretch">
                <Card solid style={{ flex: 1 }}>
                    Executive 1
                </Card>
                <Card solid style={{ flex: 1 }}>
                    Executive 2
                </Card>
                <Card solid style={{ flex: 1 }}>
                    Executive 3
                </Card>
                <Card solid style={{ flex: 1 }}>
                    Executive 4
                </Card>
            </Flex>
        </Container>

        <Container padded style={{ textAlign: 'left' }}>
            <h2 style={{ textAlign: 'center' }}>
                Board
            </h2>

            <Flex gap={16} justifyContent="stretch">
                <div style={{ flex: 1 }}>
                    <div>
                        <h3>Name</h3>
                        <p>Role</p>
                    </div>
                    <div>
                        <h3>Name</h3>
                        <p>Role</p>
                    </div>
                </div>
                <div style={{ flex: 1 }}>
                    <div>
                        <h3>Name</h3>
                        <p>Role</p>
                    </div>
                    <div>
                        <h3>Name</h3>
                        <p>Role</p>
                    </div>
                </div>
                <div style={{ flex: 1 }}>
                    <div>
                        <h3>Name</h3>
                        <p>Role</p>
                    </div>
                    <div>
                        <h3>Name</h3>
                        <p>Role</p>
                    </div>
                </div>
                <div style={{ flex: 1 }}>
                    <div>
                        <h3>Name</h3>
                        <p>Role</p>
                    </div>
                    <div>
                        <h3>Name</h3>
                        <p>Role</p>
                    </div>
                </div>
            </Flex>
        </Container>
    </main>
  )
}
