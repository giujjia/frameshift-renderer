package model;

public class ProteinResult {
	String cds_reference;
	String cds_mutation;
	String protein_reference;
	String protein_mutation;
	boolean isFrameshift;
	int firstDivergentPosition;
	int[] divergentPositions;
	int insertedBaseIndex;
	
	public ProteinResult(String cds_reference, String cds_mutation, String protein_reference, String protein_mutation, boolean isFrameshift,
			int firstDivergentPosition, int[] divergentPositions, int insertedBaseIndex) {
		this.cds_reference          = cds_reference;
		this.cds_mutation           = cds_mutation;
		this.protein_reference 		= protein_reference;
		this.protein_mutation 		= protein_mutation;
		this.isFrameshift 	 		= isFrameshift;
		this.firstDivergentPosition = firstDivergentPosition;
		this.divergentPositions 	= divergentPositions;
		this.insertedBaseIndex      = insertedBaseIndex;
	}

	public String getCdsReference() {
		return cds_reference;
	}

	public String getCdsMutation() {
		return cds_mutation;
	}

	public String getProteinReference() {
		return protein_reference;
	}

	public String getProteinMutation() {
		return protein_mutation;
	}

	public boolean isFrameshift() {
		return isFrameshift;
	}

	public int getFirstDivergentPosition() {
		return firstDivergentPosition;
	}

	public int[] getDivergentPositions() {
		return divergentPositions;
	}

	public int getInsertedBaseIndex() {
		return insertedBaseIndex;
	}
}

