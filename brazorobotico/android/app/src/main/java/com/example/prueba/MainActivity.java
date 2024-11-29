package com.example.prueba;

//clases y paquetes necesarios para que la aplicación funcione correctamente
import android.annotation.SuppressLint;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import org.json.JSONObject;

import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.charset.StandardCharsets;

public class MainActivity extends AppCompatActivity {

    /*
    Declaración de variables:
    SeekBars : Ajustar los ángulos de las articulaciones
    TextViews : Muestran el valor actual de los grados de cada articulación.
    Botón : Envía los comandos a la API.
    RadioButtons : Seleccionar "agarrar" y "soltar" para la mano.
     */

    private SeekBar seekBarCintura, seekBarHombro, seekBarCodo, seekBarMuneca, seekBarMano;
    private TextView textViewGradosCintura, textViewGradosHombro, textViewGradosCodo, textViewGradosMuneca, textViewGradosMano;
    private Button buttonEnviar;
    private RadioButton radioButtonAgarrar, radioButtonSoltar; // RadioButtons para agarrar y soltar

    private long lastToastTime = 0; // Para rastrear el tiempo del último Toast y gestionar las notificaciones que se muestran al usuario.

    @SuppressLint("MissingInflatedId")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Inicializa los elementos de la UI (interfaz de usuario)
        seekBarCintura = findViewById(R.id.seekBarCintura);
        textViewGradosCintura = findViewById(R.id.textViewGradosCintura);
        seekBarHombro = findViewById(R.id.seekBarHombro);
        textViewGradosHombro = findViewById(R.id.textViewGradosHombro);
        seekBarCodo = findViewById(R.id.seekBarCodo);
        textViewGradosCodo = findViewById(R.id.textViewGradosCodo);
        seekBarMuneca = findViewById(R.id.seekBarMuneca);
        textViewGradosMuneca = findViewById(R.id.textViewGradosMuneca);
        seekBarMano = findViewById(R.id.seekBarMano);
        textViewGradosMano = findViewById(R.id.textViewGradosMano);
        buttonEnviar = findViewById(R.id.buttonEnviar);
        radioButtonAgarrar = findViewById(R.id.radioButtonAgarrar);
        radioButtonSoltar = findViewById(R.id.radioButtonSoltar);

        // Configura cada uno de los SeekBars para cada articulación con los grados
        configurarSeekBar(seekBarCintura, textViewGradosCintura);
        configurarSeekBar(seekBarHombro, textViewGradosHombro);
        configurarSeekBar(seekBarCodo, textViewGradosCodo);
        configurarSeekBar(seekBarMuneca, textViewGradosMuneca);
        configurarSeekBar(seekBarMano, textViewGradosMano);

        // Configurar el comportamiento de los RadioButtons para que no se seleccionen al mismo tiempo
        radioButtonAgarrar.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked) {
                radioButtonSoltar.setChecked(false);
            }
        });

        radioButtonSoltar.setOnCheckedChangeListener((buttonView, isChecked) -> {
            if (isChecked) {
                radioButtonAgarrar.setChecked(false);
            }
        });

        // Configurar el botón "Enviar"
        buttonEnviar.setOnClickListener(v -> {
            gradoArticulaciones();
            accionMano();
        });
    }

    // Método para configurar el SeekBar y su TextView correspondiente
    private void configurarSeekBar(SeekBar seekBar, final TextView textView) {
        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                textView.setText(progress + "°");
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });
    }

    // Método para los grados de cada articulación, utilizando un bucle para recorrerlas
    private void gradoArticulaciones() {
        // Crear un array con los nombres de las articulaciones y sus respectivos SeekBars
        String[] articulaciones = {"cintura", "hombro", "codo", "muneca", "mano"};
        SeekBar[] seekBars = {seekBarCintura, seekBarHombro, seekBarCodo, seekBarMuneca, seekBarMano};

        // Recorrer cada articulación y su SeekBar para obtener y enviar el grado
        for (int i = 0; i < articulaciones.length; i++) { //se empieza con la variable i en 0 y va recorriendo c/arti
            int grado = seekBars[i].getProgress();  //con su respectivos grados
            enviarDatosArticulacion(articulaciones[i], grado);  //envia los datos a la api
        }
    }

    // Armado del primer JSON (formato q lee HTTP)
    private void enviarDatosArticulacion(String articulacion, int grados) {
        JSONObject jsonObject = new JSONObject();
        try {
            jsonObject.put("idCelular", "12345"); // Valor fijo
            jsonObject.put("articulacion", articulacion);
            jsonObject.put("grados", grados);

            moverBrazo(jsonObject);  // Envia los datos
        } catch (Exception e) {      // Manejo de errores
            e.printStackTrace();
        }
    }

    // 1ER POST - conexión HTTP POST a la API de "moverbrazo"
    private void moverBrazo(JSONObject jsonObject) {
        new Thread(() -> {
            try {
                URL url = new URL("http://192.168.1.26:3001/api/Brazo/mover");  // Uso de URL local
                HttpURLConnection conn = (HttpURLConnection) url.openConnection();  // Establece la Conexión HTTP
                conn.setRequestMethod("POST");  // Solicitud POST(ENVIA)
                conn.setRequestProperty("Content-Type", "application/json; charset=UTF-8");
                conn.setDoOutput(true);

                String jsonInputString = jsonObject.toString();
                try (OutputStream os = conn.getOutputStream()) {
                    byte[] input = jsonInputString.getBytes(StandardCharsets.UTF_8);
                    os.write(input, 0, input.length);
                }

                // Obtiene y Maneja la Respuesta del Servidor:
                int responseCode = conn.getResponseCode();
                if (responseCode == HttpURLConnection.HTTP_OK) {   // Si es exitosa...
                    Log.d("HttpPostRequest", "Datos enviados correctamente: " + jsonInputString);
                    runOnUiThread(() -> showToast("Datos enviados exitosamente"));
                } else {                                          // Sino... muestra un msj de error
                    Log.e("HttpPostRequest", "Error al enviar los datos: " + responseCode);
                    runOnUiThread(() -> showToast("Error al enviar datos"));
                }

                conn.disconnect();        // Se cierra la conexión HTTP para liberar los recursos.
            } catch (Exception e) {       // Manejo de Errores
                e.printStackTrace();
                runOnUiThread(() -> showToast("Error: " + e.getMessage()));
            }
        }).start();
    }

    // 2DO POST - conexión a la API de "accionbrazo" de la mano (agarrar o soltar)
    private void accionMano() {
        JSONObject jsonObject = new JSONObject();
        try {
            jsonObject.put("idCelular", "12345"); // Valor fijo
            jsonObject.put("agarrar", radioButtonAgarrar.isChecked());

            accionBrazo(jsonObject);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    // Método para enviar un JSON a la API de acción del brazo
    private void accionBrazo(JSONObject jsonObject) {
        new Thread(() -> {
            try {
                URL url = new URL("http://192.168.1.26:3001/api/Brazo/accion");
                HttpURLConnection conn = (HttpURLConnection) url.openConnection();
                conn.setRequestMethod("POST");
                conn.setRequestProperty("Content-Type", "application/json; charset=UTF-8");
                conn.setDoOutput(true);

                String jsonInputString = jsonObject.toString();
                try (OutputStream os = conn.getOutputStream()) {
                    byte[] input = jsonInputString.getBytes(StandardCharsets.UTF_8);
                    os.write(input, 0, input.length);
                }

                int responseCode = conn.getResponseCode();
                if (responseCode == HttpURLConnection.HTTP_OK) {

                    Log.d("HttpPostRequest", "Datos enviados correctamente de la acción del brazo: " + jsonInputString);
                    runOnUiThread(() -> showToast("Datos de la acción enviados exitosamente"));
                } else {
                    Log.e("HttpPostRequest", "Error al enviar los datos de la acción: " + responseCode);
                    runOnUiThread(() -> showToast("Error al enviar datos de la acción"));
                }

                conn.disconnect();
            } catch (Exception e) {
                e.printStackTrace();
                runOnUiThread(() -> showToast("Error: " + e.getMessage()));
            }
        }).start();
    }

    // Método para mostrar mensajes Toast con control de frecuencia
    private void showToast(String message) {
        long currentTime = System.currentTimeMillis();
        if (currentTime - lastToastTime > 2000) { // 2000 ms = 2 segundos
            Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
            lastToastTime = currentTime;
        }
    }
}


