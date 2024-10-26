using brazo.Application.Modelos;


namespace brazo.Application;

public class BrazoServicio : IBrazoServicio
{
    private static Queue<Mover> colaMensajes = new Queue<Mover>();   
    private static string[] articulacionesValidas = new[] { "cintura", "hombro", "codo", "muñeca", "mano" };

    public void MoverBrazo(string idCelular, string articulacion, int grados)
    {
        colaMensajes.Enqueue(Mover.CrearInstancia(articulacion, grados));
    }

    public Mover ObtenerMovimiento()
    {
        if (colaMensajes.Count() > 0)
        {
            return colaMensajes.Dequeue();
        }

        return null;
    }

    public bool ValidarDatos(string idCelular, string articulacion, int grados)
    {
            
        // Verificar que la articulación solicitada sea válida
        if (!articulacionesValidas.Contains(articulacion.ToLower()))
        {
            return false;
            //return BadRequest($"La articulación '{request.Articulacion}' no es válida. Debe ser una de las siguientes: {string.Join(", ", articulacionesValidas)}.");
        }

        // Definir los rangos de grados para cada articulación
        int minGrados = 0, maxGrados = 180;
        
        // Verificar que los grados estén dentro del rango permitido para la articulación
        if (grados < minGrados || grados > maxGrados)
        {
            return false;
        }

        return true;
   
    }

    // Método para realizar la acción de agarrar
    public bool Agarrar(string idCelular)
    {
        // Enviar la acción de "AGARRAR" para la mano
        colaMensajes.Enqueue(Mover.CrearInstancia(true)); // 0 grados podría representar la acción de cerrar la mano.
        return true;
    }

    // Método para realizar la acción de soltar
    public bool Soltar(string idCelular)
    {
        // Enviar la acción de "SOLTAR" para la mano
        colaMensajes.Enqueue(Mover.CrearInstancia(false)); // 180 grados podría representar la acción de abrir la mano.
        return true;
    }
}
