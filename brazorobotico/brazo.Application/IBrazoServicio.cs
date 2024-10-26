using brazo.Application.Modelos;

namespace brazo.Application;

public interface IBrazoServicio
{
    Mover ObtenerMovimiento();
    void MoverBrazo(string idCelular, string articulacion, int grados);
    bool ValidarDatos(string idCelular, string articulacion, int grados);
    bool Soltar(string idCelular);
    bool Agarrar(string idCelular);
}
