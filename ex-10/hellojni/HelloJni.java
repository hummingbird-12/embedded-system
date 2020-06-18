class HelloJni {
	native void printHello();
	native void printString(String str);

	static { 
		System.loadLibrary("hellojni"); 
	};

	public static void main(String args[]) {
		HelloJni jni = new HelloJni();
		jni.printHello();
		jni.printString("hello from java");
	}
}


