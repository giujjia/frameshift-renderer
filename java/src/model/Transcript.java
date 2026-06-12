package model;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class Transcript {
	//information biologic necessary
	String nmId;
	String sequence_nm;
	String npId;
	int cds_start; 
	int cds_end;
	
	public Transcript(String nmId,String sequence_nm, String npId, int cds_start, int cds_end) {
		this.nmId 		 = 	nmId;
		this.sequence_nm = 	sequence_nm;
		this.npId		 = 	npId;
		this.cds_start 	 = 	cds_start;
		this.cds_end 	 = 	cds_end;
	}
	
	public int getOrf() {
		return cds_start - 1;
	}
	public String getCdsRegion() {
		return sequence_nm.substring(this.getOrf(), cds_end);
	}
	
}
	
