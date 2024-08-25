import type { CredentialDeviceType } from "@simplewebauthn/types";

export type SerializedAuthenticator = {
    associationID: number;
    credentialID: string;
    credentialPublicKey: string;
    counter: number;
    credentialDeviceType: CredentialDeviceType;
    credentialBackedUp: boolean;
    transports?: AuthenticatorTransport[];
}