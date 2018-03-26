using System;
using System.Security.Cryptography.X509Certificates;
using RestSharp;

namespace QueryWithCertificate
{
    class Program
    {
        static void Main(string[] args)
        {
            var request = new RestRequest("api/Test/DoStuff", Method.GET);
            var client = new RestClient("https://TestServer.secure");
            var store = new X509Store(StoreName.My, StoreLocation.CurrentUser);
            store.Open(OpenFlags.ReadOnly);
            client.ClientCertificates = store.Certificates.Find(X509FindType.FindByThumbprint, "64e3578d9ae1efe2555bb982938f563b305ab354", true);
            store.Close();
            Console.WriteLine($"This application will make a request to {client.BuildUri(request).AbsolutePath} with {client.ClientCertificates.Count} certificate(s)");
            IRestResponse response = client.Execute(request);
            if (response.IsSuccessful)
            {
                Console.WriteLine("Success!");
            }
            else
            {
                Console.WriteLine($"Failure, status = {(int)response.StatusCode} {response.StatusDescription}");
            }
            Console.WriteLine("Press any key...");
            Console.ReadKey();
        }
    }
}
