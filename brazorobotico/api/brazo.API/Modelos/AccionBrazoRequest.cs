using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace brazo.API.Modelos
{
    public class AccionBrazoRequest
    {
        public string IdCelular { get; set; }
        //public string Articulacion { get; set; }
        public bool Agarrar { get; set; }
    }
}