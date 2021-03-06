// QueryWithCertificateCpp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <winhttp.h>
#include "cpprest\http_client.h"
#include <sstream>
#include <cstdio>

void PerformUnsecureRequest()
{
	web::http::uri uri(U("http://TestServer.unsecure/"));
	web::http::client::http_client client(uri);
	web::http::method method = web::http::methods::GET;
	concurrency::streams::container_buffer<std::string> body_buffer;
	bool success = false;
	client.request(method, U("/api/test/dostuff"))
		.then([](web::http::http_response response) {
		return response.content_ready();
	}).then([body_buffer, &success](web::http::http_response response) {
		std::cout << "Response status is " << response.status_code() << std::endl;
		success = response.status_code() == web::http::status_codes::OK;
		return response.body().read_to_end(body_buffer);
	}).then([body_buffer, &success](size_t bytesRead) {
		std::cout << "Response body size is " << bytesRead << std::endl;
		if (!success) return;
		std::cout << "Body is: " << body_buffer.collection() << std::endl;
	}).wait();
	std::cout << "Unsecure query completed " << (success ? "successfully" : "unsuccessfully") << "." << std::endl;
}

void PerformSecureRequest()
{
	web::http::uri uri(U("https://TestServer.secure/"));
	web::http::method method = web::http::methods::GET;
	web::http::client::http_client_config config;
	config.set_nativehandle_options([](web::http::client::native_handle handle) {
		HINTERNET hInternet = static_cast<HINTERNET>(handle);
		HCERTSTORE hStore = CertOpenSystemStore(NULL, TEXT("MY"));
		if (!hStore) throw std::exception("Bad store");
		try
		{
			uint8_t thumbprint[] = { 0x64, 0xe3, 0x57, 0x8d, 0x9a, 0xe1, 0xef, 0xe2, 0x55, 0x5b, 0xb9, 0x82, 0x93, 0x8f, 0x56, 0x3b, 0x30, 0x5a, 0xb3, 0x54 };
			CRYPT_INTEGER_BLOB hash;
			hash.cbData = 20;
			hash.pbData = thumbprint;
			PCCERT_CONTEXT cert = CertFindCertificateInStore(hStore, X509_ASN_ENCODING, 0, CERT_FIND_SHA1_HASH, &hash, NULL);
			if (!cert) throw std::exception("No certificate");
			try 
			{
				BOOL res = WinHttpSetOption(hInternet, WINHTTP_OPTION_CLIENT_CERT_CONTEXT, (LPVOID)cert, sizeof(CERT_CONTEXT));
				if (!res) {
					switch (GetLastError()) {
					case ERROR_WINHTTP_INCORRECT_HANDLE_STATE:
						break;
					case ERROR_INVALID_PARAMETER:
						break;
					case ERROR_NOT_ENOUGH_MEMORY:
						break;
					}
					throw std::exception("Failed to ser certificate on request");
				}
				CertFreeCertificateContext(cert);
			}
			catch (std::exception&)
			{
				CertFreeCertificateContext(cert);
				throw;
			}
			CertCloseStore(hStore, 0);
		}
		catch(std::exception&)
		{
			CertCloseStore(hStore, 0);
			throw;
		}
	});
	web::http::client::http_client client(uri, config);
	concurrency::streams::container_buffer<std::string> body_buffer;
	bool success = false;
	client.request(method, U("/api/test/dostuff"))
		.then([](web::http::http_response response) {
		return response.content_ready();
	}).then([body_buffer, &success](web::http::http_response response) {
		std::cout << "Response status is " << response.status_code() << std::endl;
		success = response.status_code() == web::http::status_codes::OK;
		return response.body().read_to_end(body_buffer);
	}).then([body_buffer, &success](size_t bytesRead) {
		std::cout << "Response body size is " << bytesRead << std::endl;
		if (!success) return;
		std::cout << "Body is: " << body_buffer.collection() << std::endl;
	}).wait();
	std::cout << "Unsecure query completed " << (success ? "successfully" : "unsuccessfully") << "." << std::endl;
}

int main()
{
	try {
		PerformUnsecureRequest();
		PerformSecureRequest();
	}
	catch (const std::exception& ex) {
		std::cout << "Unhandled exception" << std::endl;
		std::cout << ex.what() << std::endl;
	}
	std::cout << "Press any key to exit." << std::endl;
	getchar();
	return 0;
}

