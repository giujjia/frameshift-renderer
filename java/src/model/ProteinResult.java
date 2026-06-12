package model;

public class ProteinResult {
	String protein_reference;
	String protein_mutation;
	boolean isFrameshift;
	int firstDivergentPosition;
	int[] divergentPositions;
	
	public ProteinResult(String protein_reference, String protein_mutation, boolean isFrameshift,
			int firstDivergentPosition, int[] divergentPositions) {
		this.protein_reference 		= protein_reference;
		this.protein_mutation 		= protein_mutation;
		this.isFrameshift 	 		= isFrameshift;
		this.firstDivergentPosition = firstDivergentPosition;
		this.divergentPositions 	= divergentPositions;
	}
	
}
