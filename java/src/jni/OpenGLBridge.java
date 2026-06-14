package jni;

import model.ProteinResult;

public class OpenGLBridge {

	static {
		System.loadLibrary("motor");
	}

	public static native void sendData(
		String[] refNucleotides,
		String[] mutNucleotides,
		String[] refAminoAcids,
		String[] mutAminoAcids,
		int[] divergentPositions,
		int insertedBaseIndex,
		String transcriptId,
		String mutationHgvs
	);

	public static native void openWindow();

	public static native void moveCamera(float offsetInPixels);

	public static void prepare(ProteinResult result, String transcriptId, String mutationHgvs) {
		// Divide as sequencias de nucleotideos e aminoacidos em arrays de caracteres individuais
		String[] refNuc = result.getCdsReference().isEmpty() ? new String[0] : result.getCdsReference().split("");
		String[] mutNuc = result.getCdsMutation().isEmpty() ? new String[0] : result.getCdsMutation().split("");
		String[] refAA = result.getProteinReference().isEmpty() ? new String[0] : result.getProteinReference().split("");
		String[] mutAA = result.getProteinMutation().isEmpty() ? new String[0] : result.getProteinMutation().split("");
		int[] divPos = result.getDivergentPositions();

		sendData(refNuc, mutNuc, refAA, mutAA, divPos, result.getInsertedBaseIndex(), transcriptId, mutationHgvs);
	}
}
