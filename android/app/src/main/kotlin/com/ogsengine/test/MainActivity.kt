package com.ogsengine.test

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView

class MainActivity : AppCompatActivity() {

	@Override
	protected fun onCreate(savedInstanceState: Bundle) {
		super.onCreate(savedInstanceState)
		setContentView(R.layout.activity_main)

		// Example of a call to a native method
		val tv = findViewById(R.id.sample_text)
		tv.setText(stringFromJNI())
	}

	/**
	 * A native method that is implemented by the 'native-lib' native library,
	 * which is packaged with this application.
	 */
	external fun stringFromJNI(): String

	companion object {

		// Used to load the 'native-lib' library on application startup.
		init {
			System.loadLibrary("native-lib")
		}
	}
}