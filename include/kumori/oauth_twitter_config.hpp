#pragma once
#include "oauth_10_config.hpp"

namespace kumori
{

	struct oauth_twitter_config : oauth_10_config
	{

		explicit oauth_twitter_config(bool authorize)
		{
			site_name = "twitter";
			authorize_url = authorize ? "https://api.twitter.com/oauth/authorize" : "https://api.twitter.com/oauth/authenticate";
			access_token_url = "https://api.twitter.com/oauth/access_token";
			certificate = "\
-----BEGIN CERTIFICATE-----\n\
MIIFSTCCBDGgAwIBAgIQJLc38EsSfJ688cgjbifDoDANBgkqhkiG9w0BAQUFADCB\n\
tTELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL\n\
ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTswOQYDVQQLEzJUZXJtcyBvZiB1c2Ug\n\
YXQgaHR0cHM6Ly93d3cudmVyaXNpZ24uY29tL3JwYSAoYykxMDEvMC0GA1UEAxMm\n\
VmVyaVNpZ24gQ2xhc3MgMyBTZWN1cmUgU2VydmVyIENBIC0gRzMwHhcNMTMxMDEw\n\
MDAwMDAwWhcNMTQxMDEwMjM1OTU5WjCBhzELMAkGA1UEBhMCVVMxEzARBgNVBAgT\n\
CkNhbGlmb3JuaWExFjAUBgNVBAcUDVNhbiBGcmFuY2lzY28xFjAUBgNVBAoUDVR3\n\
aXR0ZXIsIEluYy4xGTAXBgNVBAsUEFR3aXR0ZXIgU2VjdXJpdHkxGDAWBgNVBAMU\n\
D2FwaS50d2l0dGVyLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n\
AMuy3p9WvSxhkT4BAcwOUZmTbZRgEOLOL5joBN3RSVAB05angF/xBy5gMu54XiB8\n\
0IcRPZZAeKSy2XTs9bb+CPZ+9hjmBgAJWf8APPYad3KL+ZA7XpYZ6bHAkha71Ncf\n\
YYPLPhwLvcOw7LNGsOlTyLZPZqG4gh1yjHcOvSTCkvEctqTbKrCsxRxo4WJFAlh3\n\
M+x/yulp7k1Z6bH56daQWEYnaO1I0o5cp7PcsAit7dv4VnOPUFKEvZd6/+ifBCLI\n\
QzLMYS28CPe+CEF7SPov/3ZlmZ8HNG3ymrPW9kFMn+AKwnGVNGLLrSjAB6T5TwG/\n\
b6UAMXWV3Br7xNzNbjU4lpsCAwEAAaOCAX8wggF7MBoGA1UdEQQTMBGCD2FwaS50\n\
d2l0dGVyLmNvbTAJBgNVHRMEAjAAMA4GA1UdDwEB/wQEAwIFoDAdBgNVHSUEFjAU\n\
BggrBgEFBQcDAQYIKwYBBQUHAwIwQwYDVR0gBDwwOjA4BgpghkgBhvhFAQc2MCow\n\
KAYIKwYBBQUHAgEWHGh0dHBzOi8vd3d3LnZlcmlzaWduLmNvbS9jcHMwHwYDVR0j\n\
BBgwFoAUDURcFlNEwYJ+HSCrJfQBY9i+eaUwRQYDVR0fBD4wPDA6oDigNoY0aHR0\n\
cDovL1NWUlNlY3VyZS1HMy1jcmwudmVyaXNpZ24uY29tL1NWUlNlY3VyZUczLmNy\n\
bDB2BggrBgEFBQcBAQRqMGgwJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLnZlcmlz\n\
aWduLmNvbTBABggrBgEFBQcwAoY0aHR0cDovL1NWUlNlY3VyZS1HMy1haWEudmVy\n\
aXNpZ24uY29tL1NWUlNlY3VyZUczLmNlcjANBgkqhkiG9w0BAQUFAAOCAQEAikqD\n\
F9cIZoAstD7A+gn6uHXKIvXe3Y5qMXZxrScSZUj4iZAgfMISiclIjOCKL8G/Y5jr\n\
2Z4QyvZa+5J92/mTB7sN+HokPMC+gCfo0eZe/Ew2ncg7NtiJiK+FhDHvYZ0QDzKz\n\
Bbz1iDLcEBFfocXTakbSJXxzy+2ttTfRtvffvoCqTe0efWIES+5F37JWgmyhilKt\n\
EE6SZzN50YGWGQ5+xYhL2vs5yId9NTTK57ZLbuMsVhImedIrs/7J87uJ0H1Nr8P/\n\
/5zbuXE67uRQZw0VvT6faeLImM/HRTtxDmeuz7GjJQQsy/tEgRNOlCQmh9u+08Mi\n\
J3nLzYdWUjnoBWRF5Q==\n\
-----END CERTIFICATE-----\n\
";

			request_token_url = "https://api.twitter.com/oauth/request_token";

			verify_credentials_url = "https://api.twitter.com/1.1/account/verify_credentials.json";
		}

		oauth_twitter_config(bool authorize, url callback_url_, std::string consumer_key_, std::string consumer_secret_)
			: oauth_twitter_config(authorize)
		{
			callback_url = std::move(callback_url_);
			consumer_key = std::move(consumer_key_);
			consumer_secret = std::move(consumer_secret_);
		}

		url verify_credentials_url;

	};

}
