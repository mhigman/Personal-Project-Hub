import type { CredentialDeviceType } from "@simplewebauthn/types";

export type Authenticator = {
    associationID: number;
    credentialID: Uint8Array;
    credentialPublicKey: Uint8Array;
    counter: number;
    credentialDeviceType: CredentialDeviceType;
    credentialBackedUp: boolean;
    transports?: AuthenticatorTransport[];
}