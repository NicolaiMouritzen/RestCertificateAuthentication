# RestCertificateAuthentication
Sample project to explore certificate authentication in a REST service

## Creating the required self signed certificates
To create the required certificates you need to open a command console with administrator priviliges and type the following commands.
For further information [read this](https://itq.nl/testing-with-client-certificate-authentication-in-a-development-environment-on-iis-8-5/)
The following instructions are primarily based on the following [post](https://stackoverflow.com/questions/21297139/how-do-you-sign-a-certificate-signing-request-with-your-certification-authority/21340898#21340898) 

Make the root certificate authority:
```
openssl req -x509 -config root.conf -newkey rsa:4096 -sha256 -nodes -out root.cacert.pem -outform PEM
```

This will create a root certificate that can be used as a certificate authority.
In addition to the root.cacert.pem file a privkey.pem file will be created which contains the private ket of the certificate.
I renamed the privkey.pem til root.privkey.pem.

Combine the private and public key into a single certificate file sutible for import in Windows certificate store use the following command:
```
openssl pkcs12 -export -out TestCA.pfx -inkey root.privkey.pem -in root.cacert.pem
```
This certificate must be imported into the localmachine\trusted root certificate store.


Make the server certificate making sure to correct the server names in the certificate configuration to match your intenden hostname:
```
openssl req -config Server.conf -newkey rsa:2048 -sha256 -nodes -out servercert.csr -outform PEM
```

This will create a certificate request which we can use to create our server certificate from, and a private key which we will embed in the certificate.

Next we need to create our test server certificate with the following command.
```
openssl ca -config root.server.conf -policy signing_policy -extensions signing_req -out servercert.pem -infiles servercert.csr
```

This will create the servercert.pem file which we can use to create a pfx file.
```
openss pkcs12 -export -out servercert.pfx -inkey serverkey.pem -in servercert.pem
```
This certificate must be imported into the localmachine\WebHosting certificate store.

You should now be able to create a website using the server certificate.
1. Open the IIS manager and select your website.
2. Select bindings and add an HTTPS binding.
3. If you have imported the server certificate into the WebHosting certificate store, then it should be available as a selection when you create the HTTPS binding.
4. Modify your hostfile to redirect to your website as the hostname must match the certificate DNS entries.
5. Test your website using the HTTPS binding.

Use the windows MMC with certificate snapin to verify and export the certificates.

Use the IIS manager to configure your website to require SSL and require a client certificate.
Test this setup using Chrome or IE by browsing your site, this should promt you to select a certificate from you personal certificate store with which to identify yourself.

Now you may run the C# test program to test if the connection works as intended, make sure to update the certificate thumbprint as neccessary.
