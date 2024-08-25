import Container from "@/components/Container";
import Button from "@/components/Button";
import Flex from "@/components/Flex";
import Card from "@/components/Card";

export default async function Home() {
  return (
    <main style={{ display: 'flex', flexDirection: 'column', gap: 24 }}>
      <Container style={{ borderRadius: 12, backgroundColor: '#333', height: 400 }} />
      <Container padded>
        <h1 style={{ textAlign: 'center' }}>Updates</h1>
        <Flex gap={16}>
          {[1,2,3].map(i => (
            <Card key={i} style={{ flex: 1 }}>
              <h2 style={{ margin: 0 }}>Post {i}</h2>
              <p style={{ marginTop: 8 }}>Post body</p>

              <div style={{ borderRadius: 8, height: 124, background: '#eee', marginBottom: 16 }} />

              <Flex alignItems="flex-end">
                <Button>
                  View
                </Button>
              </Flex>
            </Card>
          ))}
        </Flex>
      </Container>
      <Container style={{ display: 'flex', padding: 0 }}>
        <div style={{ flex: 1, padding: '0 48px' }}>
          <h1>About Us</h1>
          <p>Our goal is to provide to the officers of Benton County, in turn, making it a safer place.</p>
        </div>
        <div style={{ flex: 1, borderRadius: 12, backgroundColor: '#eee', padding: 36 }}>
          <p>
          The Benton County Fraternal Order of Police is a non-profit organization that aids the county’s officers through safety programs and community involvement. Since I don’t have enough information about this organization yet, I’ll just add a lot of random text here to fill up space. 

Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.
          </p>

          <Button variant='cta'>
            See more
          </Button>
        </div>
      </Container>
      <Container style={{ borderRadius: 12, backgroundColor: '#eee', padding: 48 }}>
        <h1 style={{ textAlign: 'center' }}>
          Our Programs
        </h1>

        <Flex gap={16}>
          {[1,2,3].map(i => (
            <Card solid key={i} style={{ flex: 1 }}>
              <div style={{ height: 124, background: '#eee', marginBottom: 16 }} />

              <h2 style={{ margin: 0 }}>Event {i}</h2>
              <p style={{ marginTop: 8 }}>Hello world, this is a description about this specific program or event.</p>

              <Flex alignItems="flex-end">
                <Button>
                  View
                </Button>
              </Flex>
            </Card>
          ))}
        </Flex>

        <Flex justifyContent="center">
          <Button variant='cta' style={{ marginTop: 24 }}>
            See all
          </Button>
        </Flex>
      </Container>
    </main>
  )
}
