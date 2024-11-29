



namespace brazo.Application.Modelos;

public class Mover
{
    public string Articulacion { get; private set; }

    public int Grados { get; private set; }

    public bool Agarrar { get; private set; }

    public static Mover CrearInstancia(string articulacion, int grados)
    {
        Mover mover = new Mover();
        mover.Articulacion = articulacion;
        mover.Grados = grados;

        return mover;
    }

    public static Mover CrearInstancia(bool agarrar)
    {
        Mover mover = new Mover();
        mover.Articulacion = string.Empty;
        mover.Grados = 0;
        mover.Agarrar = agarrar;

        return mover;
    }
}

