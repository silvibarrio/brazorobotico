namespace brazo.Application.Modelos
{
    public class Accion
    {
        public int IdCelular { get; private set; }           //propiedades

        public string AccionRealizada { get; private set; }

        private static readonly string[] AccionesValidas = { "AGARRAR", "SOLTAR" };  //DEF LAS acciones
                                                             // private:accesible solo dentro de la clase
                                                            // readonly: le da valor a la clase 1 sola ves
        public static Accion CrearInstancia(int idcelular, string accionrealizada)    //CI met   A clase
    {
        Accion accion = new Accion();
        accion.IdCelular = idcelular;
        accion.AccionRealizada = accionrealizada;

        return accion;
    }
    }
    public class AccionBrazoRequest
    {
        public string IdCelular { get; set; }
        public bool Cerrada { get; set; }
        
    }
}
