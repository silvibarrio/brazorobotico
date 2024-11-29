package com.example.prueba;

import android.content.Intent;
import android.os.Bundle;
import android.widget.Button;
import androidx.appcompat.app.AppCompatActivity;

public class InicioActivity extends AppCompatActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_inicio);

        // Configura el botón para ir a la 2da pantalla
        Button buttonEntrar = findViewById(R.id.buttonEntrar);
        buttonEntrar.setOnClickListener(v -> {
            Intent intent = new Intent(InicioActivity.this, MainActivity.class);
            startActivity(intent);
            finish(); // Finaliza la actividad de inicio para no regresar con el botón atrás
        });
    }
}

