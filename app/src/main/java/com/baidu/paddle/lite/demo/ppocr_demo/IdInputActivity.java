package com.baidu.paddle.lite.demo.ppocr_demo;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.baidu.paddle.lite.demo.util.Constants;

import java.io.Serializable;
import java.util.HashMap;
import java.util.Map;

public class IdInputActivity extends AppCompatActivity {

    private HashMap<String, String> resultMap;
    private EditText nameEditView;
    private EditText genderEditView;
    private EditText nationEditView;
    private EditText birthEditView;
    private EditText addressEditView;
    private EditText idEditView;

    private Button backBtn;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_id_input);

        Intent intent = this.getIntent();
        resultMap = (HashMap<String, String>) intent.getSerializableExtra(Constants.OCR_RESULT_MAP);

        initView();

        initData();

        initListener();

    }

    private void initListener() {
        backBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                IdInputActivity.this.finish();
            }
        });
    }

    private void initData() {
        nameEditView.setText(resultMap.get(Constants.NAME));
        genderEditView.setText(resultMap.get(Constants.GENDER));
        nationEditView.setText(resultMap.get(Constants.NATION));
        String birthSb = resultMap.get(Constants.BIRTH_YEAR) +
                "-" +
                resultMap.get(Constants.BIRTH_MONTH) +
                "-" +
                resultMap.get(Constants.BIRTH_DAY);
        birthEditView.setText(birthSb);
        addressEditView.setText(resultMap.get(Constants.ADDRESS));
        idEditView.setText(resultMap.get(Constants.ID));
    }

    private void initView() {
        nameEditView = findViewById(R.id.name_edit_text);
        genderEditView = findViewById(R.id.gender_edit_text);
        nationEditView = findViewById(R.id.nation_edit_text);
        birthEditView = findViewById(R.id.birth_edit_text);
        addressEditView = findViewById(R.id.address_edit_text);
        idEditView = findViewById(R.id.id_edit_text);

        backBtn = findViewById(R.id.back_btn);
    }
}