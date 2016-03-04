#pragma once
#include "oauth_20_config.hpp"

namespace kumori
{

	struct oauth_google_config : oauth_20_config
	{

		oauth_google_config()
		{
			site_name = "google";
			authorize_url = "https://accounts.google.com/o/oauth2/auth";
			access_token_url = "https://accounts.google.com/o/oauth2/token";
			certificate = "\
-----BEGIN CERTIFICATE-----\n\
MIIEgDCCA2igAwIBAgIIGh20t9hJ4MQwDQYJKoZIhvcNAQEFBQAwSTELMAkGA1UE\n\
BhMCVVMxEzARBgNVBAoTCkdvb2dsZSBJbmMxJTAjBgNVBAMTHEdvb2dsZSBJbnRl\n\
cm5ldCBBdXRob3JpdHkgRzIwHhcNMTQwMTE1MTUyOTM1WhcNMTQwNTE1MDAwMDAw\n\
WjBtMQswCQYDVQQGEwJVUzETMBEGA1UECAwKQ2FsaWZvcm5pYTEWMBQGA1UEBwwN\n\
TW91bnRhaW4gVmlldzETMBEGA1UECgwKR29vZ2xlIEluYzEcMBoGA1UEAwwTYWNj\n\
b3VudHMuZ29vZ2xlLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n\
ALWBH/KRaDTaBuQNwdeBk5Ov5TApD48cDQ5hhvPyrU41A7OYJ/LTZOAiGtOc8uk9\n\
EgHuzfhk8Ga6YVsrB0p5J2nlYQQJB/ECcKMnIRuijkr5y1zaCr6YGunM7J0pEfkB\n\
kn2gBLnVIvryf5SHj/FjbXjK3ohgnNNvu3a7qfOhdYknUD++ubTiEfgaF9ZqvOyg\n\
O9N+i8O5dkkifhLaatqwNwa+xzuQV0V3vWc60nKS4o/64h1gLWmoCZRJkheNWLBe\n\
OcRxI6CYBFne6kSaz9DZpvS6AtefqI0A6R21IYkVUgBdxIBIrnLwdbsmVreGAhS/\n\
m0hg7webBGSD6tWgqFhQUzcCAwEAAaOCAUYwggFCMB0GA1UdJQQWMBQGCCsGAQUF\n\
BwMBBggrBgEFBQcDAjAeBgNVHREEFzAVghNhY2NvdW50cy5nb29nbGUuY29tMGgG\n\
CCsGAQUFBwEBBFwwWjArBggrBgEFBQcwAoYfaHR0cDovL3BraS5nb29nbGUuY29t\n\
L0dJQUcyLmNydDArBggrBgEFBQcwAYYfaHR0cDovL2NsaWVudHMxLmdvb2dsZS5j\n\
b20vb2NzcDAdBgNVHQ4EFgQUY8YclpkTPhmXXnlyLKNE/dC6oykwDAYDVR0TAQH/\n\
BAIwADAfBgNVHSMEGDAWgBRK3QYWG7z2aLV29YG2u2IaulqBLzAXBgNVHSAEEDAO\n\
MAwGCisGAQQB1nkCBQEwMAYDVR0fBCkwJzAloCOgIYYfaHR0cDovL3BraS5nb29n\n\
bGUuY29tL0dJQUcyLmNybDANBgkqhkiG9w0BAQUFAAOCAQEAAAdoVsN0pjMivD/C\n\
ksQESrFqFXgWYgacUsV4avPFL6X5HYC3rltRv/GzFI5oq+7Mc9Zokrq6OzuLzhCz\n\
4a+M3HssN9+V700c4+EyL2oSgSXZM40YxHVrp3v67GUFQKICTRUdDy3He42c2Wy6\n\
s4GNe/cG5ggw1vKRMtRDdlWVo4yWs20eVPZB2gdHpm7efl/2l0Q29/havbsA8r/z\n\
47b9Hh8/huroc6iR8pKgd8SHcLhCQzqxmEbrfWKVYrKfN2e0PdIc/HC+sR06Rl92\n\
WxPocfVmhSl+IGZ6dzJ9VfbVMnAkDXM+u8z3vGChAfTPQQe+NNVwHpuX0GvYkYBs\n\
7Pb00A==\n\
-----END CERTIFICATE-----\n\
";

			me_url = "https://www.googleapis.com/plus/v1/people/me";
			api_certificate = "\
-----BEGIN CERTIFICATE-----\n\
MIIEvDCCA6SgAwIBAgIIfl8mVF+G0nYwDQYJKoZIhvcNAQEFBQAwSTELMAkGA1UE\n\
BhMCVVMxEzARBgNVBAoTCkdvb2dsZSBJbmMxJTAjBgNVBAMTHEdvb2dsZSBJbnRl\n\
cm5ldCBBdXRob3JpdHkgRzIwHhcNMTQwMTE1MTQzNzQwWhcNMTQwNTE1MDAwMDAw\n\
WjBqMQswCQYDVQQGEwJVUzETMBEGA1UECAwKQ2FsaWZvcm5pYTEWMBQGA1UEBwwN\n\
TW91bnRhaW4gVmlldzETMBEGA1UECgwKR29vZ2xlIEluYzEZMBcGA1UEAwwQKi5n\n\
b29nbGVhcGlzLmNvbTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAIql\n\
lZCGE5IQgsLSj7S4jYKDfrOkf+qicEOdCl5irCQfls2+h+rVSs4HCWTETq2zktii\n\
DHzFty0oMKSYKSK1EP6wtgCFYawiZxOcX7u+W+qqbnpuManu4C9pW0+mW/CCDf8N\n\
RFDlTXpUaWPh3BRJPKOze91FIJbLy53gnuUyszUrWL6eP9p9ZZTe15RKcdGZ/U5r\n\
Tax/BSBIvvEeM7mhG9sjMTlNIEs5cACAogI/VflY1iCeCROMG7hAbVItc84/4/ck\n\
aIcQCNFqeJfwlxyAk2vMpPEeURGqIxiKR9jruy+SEPq9LlWhrG6g2ne4lj3ETpIj\n\
3xUuDWUbJXUYPuzChMMCAwEAAaOCAYUwggGBMB0GA1UdJQQWMBQGCCsGAQUFBwMB\n\
BggrBgEFBQcDAjBdBgNVHREEVjBUghAqLmdvb2dsZWFwaXMuY29tghgqLmNsb3Vk\n\
ZW5kcG9pbnRzYXBpcy5jb22CFmNsb3VkZW5kcG9pbnRzYXBpcy5jb22CDmdvb2ds\n\
ZWFwaXMuY29tMGgGCCsGAQUFBwEBBFwwWjArBggrBgEFBQcwAoYfaHR0cDovL3Br\n\
aS5nb29nbGUuY29tL0dJQUcyLmNydDArBggrBgEFBQcwAYYfaHR0cDovL2NsaWVu\n\
dHMxLmdvb2dsZS5jb20vb2NzcDAdBgNVHQ4EFgQUmx/qEC9DTs8lsifFqQtMQNE9\n\
iTgwDAYDVR0TAQH/BAIwADAfBgNVHSMEGDAWgBRK3QYWG7z2aLV29YG2u2IaulqB\n\
LzAXBgNVHSAEEDAOMAwGCisGAQQB1nkCBQEwMAYDVR0fBCkwJzAloCOgIYYfaHR0\n\
cDovL3BraS5nb29nbGUuY29tL0dJQUcyLmNybDANBgkqhkiG9w0BAQUFAAOCAQEA\n\
a5uNYbV5BOfOe5x4WaaZBOj5gYV/W3w8jy6adCOu9l/9OacWPbeB5p0abQYRfKUP\n\
rgSZmY+AHODqR1QUl+wJv5WKolp8zDjl93m75HEbAJH1PeN4nBUZLnNd7wvZvM6d\n\
8mfysocuXnRAEP0U7jijLlW0tXmQj4DKfil/lyMQrQx6mb6067kVp6xrPWjPaQkQ\n\
WKhZ+1hR0uIuLA/8InVY5eRnBuSkPok2NeqpwIqS5oKQSeb32L5lgGttZX6/5fqH\n\
J58oscZhaYnkpP/E/kloVC1SeIfAyQhuIZSpQH8otXOS2WGoeidAHDfJD4hzOkHb\n\
0WtZMMikUKop0BICGBQ6JQ==\n\
-----END CERTIFICATE-----\n\
";
		}

		oauth_google_config(std::string callback_url_, std::string client_id_, std::string client_secret_)
			: oauth_google_config()
		{
			callback_url = std::move(callback_url_);
			client_id = std::move(client_id_);
			client_secret = std::move(client_secret_);
		}

		url me_url;
		std::string api_certificate;

	};

}
