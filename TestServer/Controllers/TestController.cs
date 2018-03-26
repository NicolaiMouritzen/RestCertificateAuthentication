using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Web.Http;

namespace TestServer.Controllers
{
    public class TestController : ApiController
    {
        [HttpGet]
        [Route("api/test/DoStuff")]
        public IHttpActionResult DoStuff()
        {
            return Content(HttpStatusCode.OK, "You have access");
        }
    }
}
