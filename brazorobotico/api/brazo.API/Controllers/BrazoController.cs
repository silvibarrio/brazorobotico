using brazo.API.Modelos;
using brazo.Application;
using brazo.Application.Modelos;
using Microsoft.AspNetCore.Mvc;

namespace brazo.API.Controllers
{
    [ApiController]
    [Route("api/[controller]")]
    public class BrazoController : ControllerBase
    {
        private readonly ILogger<BrazoController> _logger;

        private readonly IBrazoServicio _brazoServicio;

        public BrazoController(IBrazoServicio brazoServicio, ILogger<BrazoController> logger)
        {
            _logger = logger;
            _brazoServicio = brazoServicio;
        }

        // POST /api/brazo/mover
        [HttpPost("mover")]
        public IActionResult MoverBrazo([FromBody] MoverBrazoRequest request)
        {
            if (request == null || string.IsNullOrEmpty(request.IdCelular) || string.IsNullOrEmpty(request.Articulacion))
            {
                return BadRequest("Datos inválidos.");
            }

            if (!_brazoServicio.ValidarDatos(request.IdCelular, request.Articulacion, request.Grados))
            {
                return BadRequest("Datos inválidos.");
            }
            
           _logger.LogInformation($"Mover articulación: {request.Articulacion}, Grados: {request.Grados} para IdCelular: {request.IdCelular}");

           // Aquí puedes implementar la lógica para mover la articulación del brazo
           _brazoServicio.MoverBrazo(request.IdCelular, request.Articulacion, request.Grados);

           return Ok(new { message = $"Articulación {request.Articulacion} movida {request.Grados} grados con éxito." });
        }

        // POST /api/brazo/accion

        [HttpPost("accion")]
        public IActionResult AccionBrazo([FromBody] brazo.API.Modelos.AccionBrazoRequest request) 
        {
            if (request == null || string.IsNullOrEmpty(request.IdCelular) )
            {
                return BadRequest("Datos inválidos.");
            }

            // Procesar la acción de agarrar y soltar para la mano
            if (request.Agarrar)
            {
                _brazoServicio.Agarrar(request.IdCelular);
                _logger.LogInformation($"Acción AGARRAR realizada para IdCelular: {request.IdCelular}");
                return Ok(new { message = "Acción de AGARRAR realizada con éxito." });
            }
            else 
            {
                _brazoServicio.Soltar(request.IdCelular);
                _logger.LogInformation($"Acción SOLTAR realizada para IdCelular: {request.IdCelular}");
                return Ok(new { message = "Acción de SOLTAR realizada con éxito." });
            }
            
            
        }

       
        // GET /api/brazo/comando
        [HttpGet("comando")]
        public IActionResult ObtenerComando()
        {
            
            // Aquí puedes implementar la lógica para obtener el estado del comando
            var movimiento = _brazoServicio.ObtenerMovimiento();
            
            if (movimiento == null)
            {
                return Ok();
            }

            return Ok(movimiento);
        }
    }

    
}

