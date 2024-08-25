import uint8ToBase64, { base64ToUint8 } from "./uint8ToBase64"

export const serializeCredentials = (obj: any) => {
    let newObj: any = {
        _ID: obj.id,
        _RAW_ID: uint8ToBase64(new Uint8Array(obj.rawId)),
        _BUFFER: uint8ToBase64(new Uint8Array(obj.response.clientDataJSON)),
        _ATTEST: uint8ToBase64(new Uint8Array(obj.response.attestationObject)),
        _EXT: obj.getClientExtensionResults()
    }

    return newObj

    // for (let key of Object.keys(obj)) {
    //     if (obj[key] instanceof ArrayBuffer)
    //         newObj[key] = {
    //             type: 'ArrayBuffer',
    //             value: uint8ToBase64(new Uint8Array(obj[key]))
    //         };
    //     else if (typeof(obj[key]) == 'object')
    //         newObj[key] = serializeCredentials(obj[key])
    //     else
    //         newObj[key] = obj[key]
    // }

    // return newObj
}

export const deserializeCredentials = (obj: any) => {
    return {
        id: obj._ID,
        rawId: base64ToUint8(obj._RAW_ID),
        response: {
            attestationObject: base64ToUint8(obj._ATTEST),
            clientDataJSON: base64ToUint8(obj._BUFFER)
        },

        getClientExtensionResults() {
            return obj._EXT;
        }
    }
}
