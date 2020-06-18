public class JniTest {
	native public void callNative2(int num);

	private int intField;

	public JniTest(int num) {
		intField = num;
		System.out.println("java JniTest "+num);
	}

	public int callByNative(int num) {
		System.out.println("java callByNative "+num);
		return num;
	}

	public void callTest() {
		System.out.println("java callTest "+intField);
	}

	public void callTest2() {
		System.out.println("java callTest2 ");
		callNative2(10);
	}
	public void callByNative2(int num) {
		System.out.println("java callByNative2 "+num);
	}
	 
}
