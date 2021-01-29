
package com.Craft_Java;

import android.app.Activity;
import android.widget.TextView;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.view.MotionEvent;
import android.util.Log;
import android.app.AlertDialog;
import android.content.DialogInterface;


public class Craft_Java extends Activity implements View.OnTouchListener, DialogInterface.OnClickListener
{
    GameView mView;
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_activity);

        mView = (GameView)findViewById(R.id.glView);

        Button b = (Button)findViewById(R.id.btn_remove);
        b.setOnTouchListener(this);

        b = (Button)findViewById(R.id.btn_add);
        b.setOnTouchListener(this);

        b = (Button)findViewById(R.id.btn_left);
        b.setOnTouchListener(this);

        b = (Button)findViewById(R.id.btn_right);
        b.setOnTouchListener(this);

        b = (Button)findViewById(R.id.btn_up);
        b.setOnTouchListener(this);

        b = (Button)findViewById(R.id.btn_down);
        b.setOnTouchListener(this);

        b = (Button)findViewById(R.id.btn_fly);
        b.setOnTouchListener(this);

        b = (Button)findViewById(R.id.btn_jump);
        b.setOnTouchListener(this);

        b = (Button)findViewById(R.id.btn_change_texture);
        b.setOnTouchListener(this);
    }

    @Override 
    protected void onPause() {
        super.onPause();
        mView.onPause();
    }

    @Override 
    protected void onResume() {
        super.onResume();
        mView.onResume();
    }

    @Override 
    protected void onStop() {
        super.onStop();
        NativeLib.terminate();
    }

    @Override
    protected void onDestroy(){
        super.onDestroy();
    }

    boolean b_fly = false; 
    @Override
    public boolean onTouch(View v, MotionEvent event) {
        if(event.getAction() == MotionEvent.ACTION_DOWN)
        {
    	    if(v.getId() == R.id.btn_remove){
                NativeLib.onButtonClick(0); // remove button
    	    }
    	    else if(v.getId() == R.id.btn_add){
                
                NativeLib.onButtonClick(1); // add button
    	    }
            else if( v.getId() == R.id.btn_fly)
            {
                NativeLib.onButtonClick(2); //  fly button
                Button b = (Button)v;
                b_fly = !b_fly;
                if( b_fly )
                    b.setBackgroundResource(R.drawable.rating_hover);
                else 
                    b.setBackgroundResource(R.drawable.rating);
            }
            else if(v.getId() == R.id.btn_change_texture){
                NativeLib.onButtonClick(3); //  change texture
            }

            if(v.getId() == R.id.btn_left){
                NativeLib.onArrow(0);
            }
            else if(v.getId() == R.id.btn_right){
                NativeLib.onArrow(1);
            }
            else if(v.getId() == R.id.btn_up){
                NativeLib.onArrow(2);
            }
            else if(v.getId() == R.id.btn_down){
                NativeLib.onArrow(3);
            }
            else if( v.getId() == R.id.btn_jump ) // jump button
            {
                NativeLib.onArrow(4);
                //NativeLib.onButtonClick(3); 
            }
        }
        else if(event.getAction() == MotionEvent.ACTION_UP){
            if(v.getId() == R.id.btn_left || v.getId() == R.id.btn_right || v.getId() == R.id.btn_up || v.getId() == R.id.btn_down || v.getId() == R.id.btn_jump) 
                NativeLib.onArrow(-1);
        }

        
        Log.e("BbanJjak", "action = " + event.getAction());
        return false;
    }

    @Override
    public void onBackPressed()
    {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setMessage("Do you want to quit?");
        builder.setPositiveButton("Yes", this);
        builder.setNegativeButton("No", this);
        builder.show();
        //super.onBackPressed();
    }

    @Override
    public void onClick(DialogInterface dialog, int which) {
        switch (which){
            case DialogInterface.BUTTON_POSITIVE:
                super.onBackPressed();
                break;

            case DialogInterface.BUTTON_NEGATIVE:
                break;
        }
    }
}
