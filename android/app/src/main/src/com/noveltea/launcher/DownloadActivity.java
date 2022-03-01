package com.noveltea.launcher;

import com.noveltea.launcher.R;
import android.app.NativeActivity;
import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Intent;
import android.content.DialogInterface;
import android.os.Bundle;
import android.net.Uri;


public class DownloadActivity extends Activity {
	
	public void launch(String fileName) {
		Intent intent = new Intent()
			.setClassName("com.noveltea.launcher", "android.app.NativeActivity")
			.putExtra("projectFile", fileName);
		startActivity(intent);
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		final Intent intent = getIntent();
		final String action = intent.getAction();
		Uri uri = intent.getData();
		String s = intent.getDataString();
	
		ProgressDialog dialog = new ProgressDialog(DownloadActivity.this);
		dialog.setIconAttribute(android.R.attr.alertDialogIcon);
		dialog.setTitle(s);
		dialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
		dialog.setMax(100);
		dialog.setButton(DialogInterface.BUTTON_NEGATIVE,
				getText(R.string.alert_dialog_cancel), new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
				DownloadActivity.this.finish();
				launch("/path/file.ntp");
			}
		});
		dialog.show();
	}
	
}
