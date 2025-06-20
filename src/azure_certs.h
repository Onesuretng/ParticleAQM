// X.509 Certificate Configuration for Azure IoT Hub
// This file contains the device certificate and private key for TLS authentication
// Generated from certificate files in the certs/ directory
//Ignore

#ifndef AZURE_CERTS_H
#define AZURE_CERTS_H

// Certificate thumbprint (SHA-1 fingerprint)
const String DEVICE_CERT_THUMBPRINT = "0CFBE92F9761FC0525B029E09B8374A1295E4890";

// Root CA Certificate (Baltimore CyberTrust Root or DigiCert Global Root G2)
extern const char AZURE_IOT_ROOT_CA[];

// Device X.509 Certificate (PEM format)
extern const char AZURE_IOT_CLIENT_CERT[];

// Device Private Key (PEM format)  
extern const char AZURE_IOT_CLIENT_KEY[];


// Device Certificate (PEM format)
// This is the public certificate that was uploaded to Azure IoT Hub
const char DEVICE_CERTIFICATE[] = 
"-----BEGIN CERTIFICATE-----\n"
"MIIDtTCCAp2gAwIBAgIUBi4dOmm8TkeZ5aYpeUnDuWAATS8wDQYJKoZIhvcNAQEL\n"
"BQAwgYIxCzAJBgNVBAYTAlVTMRUwEwYDVQQIDAxOb3J0aCBEYWtvdGExDzANBgNV\n"
"BAcMBkhvcmFjZTEQMA4GA1UECgwHVGl0YW5CSTEQMA4GA1UEAwwHVGl0YW5CSTEn\n"
"MCUGCSqGSIb3DQEJARYYcHJvbWV0aGV1c18yQGhvdG1haWwuY29tMB4XDTI1MDYx\n"
"NjIxMjEwM1oXDTI2MDYxNjIxMjEwM1owgYIxCzAJBgNVBAYTAlVTMRUwEwYDVQQI\n"
"DAxOb3J0aCBEYWtvdGExDzANBgNVBAcMBkhvcmFjZTEQMA4GA1UECgwHVGl0YW5C\n"
"STEQMA4GA1UEAwwHVGl0YW5CSTEnMCUGCSqGSIb3DQEJARYYcHJvbWV0aGV1c18y\n"
"QGhvdG1haWwuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAn0Qm\n"
"2lWHVSfLaKQwWWKHhup+ylEqCkN7KA95VnTcjjLUCF0kHdV2SM9v4aMGYibSEs24\n"
"jZAjTh/XBKL14SeBnBCrU82kJohLj9D6TYm8iEkHsHM2qT/vgEmqAU8l3M8/9i1O\n"
"l50wnmCXrYKrZ0JI34HiHdOderWSMf2AeYx6jD5C6Uh9i9foT1ZHSFfkGL2+vRAs\n"
"CFYVnGegpzxTZ5+2O44BoXHYY+aJrHRtfXpL6DUS0nZAqLg7T0oCMaM2nhB9AyMt\n"
"gCjbD1fk/R40EW3NCy25fnz55fGZsMjd3uUeEmbDtUo1Q1BpcVNXIH2D6w04KlXa\n"
"dbidGMfQQDT0p9R9ZwIDAQABoyEwHzAdBgNVHQ4EFgQU7O/tVrv+Qk3zRt9qSx82\n"
"gLxMHdUwDQYJKoZIhvcNAQELBQADggEBAAG4itV7B97SSUU7CJPUwIk22ZTxUYw9\n"
"DgxgTP+zvS4Z7Ey2MmkdCOXYdub20enxF9emKjcxIuYlPHK+N1zK9PVs1AC22JJI\n"
"JE6zGHcnAQo8DFvsUbfiexJt0t3Il09kBT6CJE5Dsn1E85JWr9JXESwDpC6M2fl2\n"
"Sq3hl4QgyRniuq6duvQlRWOetM4/re14yRAfuG9NpN1+N7GO9yD7wGV/KrZMw7UY\n"
"mu1lcq5ge+kvTfGkK7MgB+k6IceK+Fzvlz1Q2hFjEG5gvmCoQN27+HD0RmNn48J9\n"
"m4D1vGqGGrCJqcV2v2azDLEwn4tfsLPR5U72YMDqPcvEXPNUNzqkbmo=\n"
"-----END CERTIFICATE-----\n";

// Device Private Key (PEM format)
// This is the private key corresponding to the certificate above
// WARNING: Keep this secure and never share it
const char DEVICE_PRIVATE_KEY[] = 
"-----BEGIN PRIVATE KEY-----\n"
"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCfRCbaVYdVJ8to\n"
"pDBZYoeG6n7KUSoKQ3soD3lWdNyOMtQIXSQd1XZIz2/howZiJtISzbiNkCNOH9cE\n"
"ovXhJ4GcEKtTzaQmiEuP0PpNibyISQewczapP++ASaoBTyXczz/2LU6XnTCeYJet\n"
"gqtnQkjfgeId0516tZIx/YB5jHqMPkLpSH2L1+hPVkdIV+QYvb69ECwIVhWcZ6Cn\n"
"PFNnn7Y7jgGhcdhj5omsdG19ekvoNRLSdkCouDtPSgIxozaeEH0DIy2AKNsPV+T9\n"
"HjQRbc0LLbl+fPnl8ZmwyN3e5R4SZsO1SjVDUGlxU1cgfYPrDTgqVdp1uJ0Yx9BA\n"
"NPSn1H1nAgMBAAECggEATCfQ8j7KfxAPxiIYkm04Q+2WIhTsZsr8tVus2Af6PK6W\n"
"T8eF0RQ0wr0aEQkcsMtCUUZ4dIyTLTSxYqCvtPFomuEno2NmFhy/EO5K+c+eTIar\n"
"PBbjcMvHMI9I0OZxQuQBACyK85lw2ZJzhL7CShZadTGMl1mMaFQxK2E90SE4qh0M\n"
"ToOqGU+ZIyMLAatx1HIBLQnXQX6KzVeHot+2+9R98fKEymB6WfbuqoQXyqEg4aSf\n"
"I/oN80F2ZHPuElRAggq0DCd3XYYLPYLeiURrxhDZDk2Qk7yLxXSnJr/Wv4iZIXSi\n"
"hQhf6RDZqmz6MEwMeo8ncQeP9ElW50v81DMznN83bQKBgQDM0Bwn/6uHoB/mhFP+\n"
"T6qZs6uAPRD3NYnfqINP11hLe5hhvl9sKDgyVG05UcN7Yqx58mafAgC3pRkoCklQ\n"
"9UJP6906tGLoUaC2jvBKbg5rS4AKqIE2oHMSthUTH4628LzOGau+q1UtxwZMUq3n\n"
"r6kCPvnUbmLcDMKGz2wo8/rSFQKBgQDHEfjqsQXNsPrERrJ3vn9DFy6+iADC3Aqd\n"
"ZQ3zEducYcHqh3ccB3QXFPQSwdjTnIV/rkwXfQSssedoTXUeMxlKX9K5BRnPcBAi\n"
"dRlUiANgRNabb5H36+VY23KDNAfY4c7JOUFie1WOpRzFGDSMwkAZV57rsEQaZ2GH\n"
"DPL/uym8iwKBgDCqMkZ+xW8QaQQI5TxECbFDs7hgyiszGXN/QtQxwtUMpQR0i4/T\n"
"bqbm0J0SBHTI2VqQcjnN7AW2wOd2qj0GSZ9b19EfE3FIE/vMsuO8isiLw/zEVQ95\n"
"yF3lzdQhZ3Ood/QwvVIsu6yI4cI40wF+B5Z1CD+liu/mSEJn/rcsYaiRAoGBAIPJ\n"
"/yuIdFofT3S/hQlHQUMuRtD2guJ+AD0BGMIR0XzvLwzQD+ere4yIX6lHUf+OAMdt\n"
"oYxvRLf7s0JW/TE4DQcy9vNsUBEHlHbrZRCUVv/VsuoGa/Fg513zMhVwTb2kiFyp\n"
"HgNPuW2NG8XT535EX2Eno9tOv1dnl6ZryTfKNbBPAoGAKDmIZZROCOwn1gZiF1hL\n"
"f76V4/YIOP/gXiTTXWU42R1rPIABXPkM4AvtkA9j55GwF6SE2+41xBhtKRr6OgdK\n"
"IYLYq3j6wRQ773Zu+4xXWAC7A9zwSZVaC7za7yYZW/tSdIJZzaFqcl4hJsEaM4k7\n"
"tAt4yO+Nme7GVI18fT5tMoQ=\n"
"-----END PRIVATE KEY-----\n";

// Certificate Information
const char DEVICE_CERT_SUBJECT[] = "CN=TitanBI,O=TitanBI,L=Horace,ST=North Dakota,C=US,emailAddress=prometheus_2@hotmail.com";
const char DEVICE_CERT_ISSUER[] = "CN=TitanBI,O=TitanBI,L=Horace,ST=North Dakota,C=US,emailAddress=prometheus_2@hotmail.com";

// Certificate validity (extracted from the certificate)
const char CERT_NOT_BEFORE[] = "2025-06-16T21:21:03Z";
const char CERT_NOT_AFTER[] = "2026-06-16T21:21:03Z";

// Function to get certificate thumbprint/fingerprint
// This should match the thumbprint shown in Azure IoT Hub
String getCertificateThumbprint() {
    return DEVICE_CERT_THUMBPRINT;
}

#endif // AZURE_CERTS_H
