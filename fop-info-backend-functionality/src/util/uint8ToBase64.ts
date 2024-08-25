export default function uint8ToBase64(bytes: Uint8Array){
    let binary = '';
    let len = bytes.byteLength;
    for (let i = 0; i < len; i++) {
        binary += String.fromCharCode(bytes[i]);
    }
    return btoa(binary);
}

export function base64ToUint8(b64: string) {
    let binary = atob(b64);
    let result = [];
    for (let i = 0; i < binary.length; i++) {
        result.push(binary.charCodeAt(i))
    }

    return Uint8Array.from(result);
}
