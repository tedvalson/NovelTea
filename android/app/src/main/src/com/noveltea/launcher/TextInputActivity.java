package com.noveltea.launcher;

import com.noveltea.launcher.R;
import android.app.NativeActivity;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.NativeActivity;
import android.content.Intent;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;


public class TextInputActivity extends Activity {
	static {
		System.loadLibrary("noveltea-launcher");
	}
	
	public static void launch(String message, int ref, NativeActivity activity) {
		Intent intent = new Intent(activity, TextInputActivity.class)
			.putExtra("message", message)
			.putExtra("ref", ref);
		activity.startActivity(intent);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Intent i = getIntent();
		showAlert(i.getStringExtra("message"), i.getIntExtra("ref", 0));
	}

	public void showAlert(final String message, final int ref) {
		this.runOnUiThread(new Runnable() {
			public void run() {
				AlertDialog.Builder builder = new AlertDialog.Builder(TextInputActivity.this);
				builder.setTitle(message);
				
				LayoutInflater factory = LayoutInflater.from(TextInputActivity.this);
				final View textEntryView = factory.inflate(R.layout.dialog, null);
				final EditText input = (EditText) textEntryView.findViewById(R.id.input);
				builder.setView(textEntryView);
				DialogInterface.OnClickListener listener = new DialogInterface.OnClickListener() {
					public void onClick(DialogInterface d, int id) {
						String s = input.getText().toString();
						if (s.isEmpty()) {
							showAlert(message, ref);
							return;
						}
						showAlertCallback(s, ref);
						TextInputActivity.this.finish();
					}
				};
				builder.setPositiveButton(android.R.string.ok, listener);
				builder.setCancelable(false);
				AlertDialog dialog = builder.create();
				dialog.show();
			}
		});
	}

	public native void showAlertCallback(String s, int ref);
}
