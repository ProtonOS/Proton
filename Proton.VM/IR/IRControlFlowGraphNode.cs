using System;
using System.Collections;
using System.Collections.Generic;

namespace Proton.VM.IR
{
	public sealed class IRControlFlowGraphNode
	{
		public int Index = 0;
		public List<IRInstruction> Instructions = new List<IRInstruction>();
		public List<IRControlFlowGraphNode> ChildNodes = new List<IRControlFlowGraphNode>();
		public List<IRControlFlowGraphNode> ParentNodes = new List<IRControlFlowGraphNode>();
		public BitArray Dominators = null;
		public int DominatorsCount = 0;
		public IRControlFlowGraphNode Dominator = null;
		//public List<Node> SourceFrontiers = new List<Node>();
		//public List<Node> DestinationFrontiers = new List<Node>();
		public IRLocal[] SSAFinalIterations = null;
		public HashSet<IRLocal>[] SSAPhiSources = null;

		public IRControlFlowGraphNode(int pIndex) { Index = pIndex; }

		public void LinkTo(IRControlFlowGraphNode pChildNode)
		{
			ChildNodes.Add(pChildNode);
			pChildNode.ParentNodes.Add(this);
		}
	}
}
