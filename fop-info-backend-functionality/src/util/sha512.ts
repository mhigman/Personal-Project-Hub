//NOTE: crypto.subtle only works in https or localhost contexts.


export async function sha512(message: string, salt: string = "") {
    // encode as UTF-8

    const blockSize = Math.floor(message.length/3)

    const blocksToHash = [await sha512Block(message.substring(0, blockSize) + salt), 
                          await sha512Block(message.substring(blockSize, 2*blockSize) + salt),
                          await sha512Block(message.substring(2*blockSize) + salt)];

    return blocksToHash.join("");
}

async function sha512Block(value: string) {
    const msgBuffer = new TextEncoder().encode(value);                    

    // hash the message
    const hashBuffer = await crypto.subtle.digest('SHA-512', msgBuffer);

    // convert ArrayBuffer to Array
    const hashArray = Array.from(new Uint8Array(hashBuffer));

    // convert bytes to hex string                  
    const hashHex = hashArray.map(b => b.toString(16).padStart(2, '0')).join('');
    return hashHex;
}