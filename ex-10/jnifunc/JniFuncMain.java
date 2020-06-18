public class JniFuncMain{
	private static int staticIntField = 300;
	static native JniTest createJniObject();

	static { System.loadLibrary("jnifunc"); };

	public static void main(String args[] ) {
		JniTest t = createJniObject();
		t.callTest();
		t.callTest2();
	}
}
