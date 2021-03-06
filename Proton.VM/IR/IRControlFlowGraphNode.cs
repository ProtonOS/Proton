﻿using System;
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
		public BitVector Dominators = null;
		public int DominatorsCount = 0;
		public IRControlFlowGraphNode Dominator = null;
		public List<IRControlFlowGraphNode> Frontiers = new List<IRControlFlowGraphNode>();
		public Tuple<IRLocal, bool>[] SSAFinalIterations = null;
		public IRLocal[] SSAPhis = null;

		public IRControlFlowGraphNode(int pIndex) { Index = pIndex; }

		public void LinkTo(IRControlFlowGraphNode pChildNode)
		{
			ChildNodes.Add(pChildNode);
			pChildNode.ParentNodes.Add(this);
		}

		public override string ToString()
		{
			return "Node #" + Index.ToString();
		}
	}
}
