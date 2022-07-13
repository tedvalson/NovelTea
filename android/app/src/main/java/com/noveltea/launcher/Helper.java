package com.noveltea.launcher;

import com.noveltea.launcher.R;
import android.app.NativeActivity;
import android.content.Intent;
import android.util.DisplayMetrics;


public class Helper {
	
	public static void launch(String message, int ref, NativeActivity activity) {
		Intent intent = new Intent(activity, TextInputActivity.class)
			.putExtra("message", message)
			.putExtra("ref", ref);
		activity.startActivity(intent);
	}
	
	public static DisplayMetrics getDPI(NativeActivity activity) {
		return activity.getResources().getDisplayMetrics();
	}
	
	public static String getProjectFileName(NativeActivity activity) {
		return activity.getIntent().getStringExtra("projectFile");
	}
	
}
