// http://www.codeproject.com/KB/recipes/matheval.aspx

using System;
using System.Reflection;

namespace MathEval
{
    public class MathEvaluator
    {
        public static void Main(string[] args)
        {
            MathExpressionParser mp = new MathExpressionParser(); 
            mp.init("Math.Sin(x)*y");

            var result = mp.eval(Math.PI / 6, 10);
        }
    }

    public class MyClassBase { public MyClassBase() { } public virtual double eval(double x, double y) { return 0.0; } }

    public class MathExpressionParser 
    { 
        MyClassBase myobj = null; 
        
        public MathExpressionParser() { } 
        
        public bool init(string expr) 
        { 
            Microsoft.CSharp.CSharpCodeProvider cp = new Microsoft.CSharp.CSharpCodeProvider(); 
            System.CodeDom.Compiler.ICodeCompiler ic = cp.CreateCompiler(); 
            System.CodeDom.Compiler.CompilerParameters cpar = new System.CodeDom.Compiler.CompilerParameters(); 
            cpar.GenerateInMemory = true; 
            cpar.GenerateExecutable = false; 
            cpar.ReferencedAssemblies.Add("system.dll");
            cpar.ReferencedAssemblies.Add("DynamicAssemblyTest01.exe"); 
            string src = "using System;" + 
                "class myclass:MathEval.MyClassBase" + 
                "{" + "public myclass(){}" + 
                "public override double eval(double x,double y)" + 
                "{" + 
                "return " + expr + ";" + 
                "}" + "}"; 
            
           System.CodeDom.Compiler.CompilerResults cr = ic.CompileAssemblyFromSource(cpar, src); 
            
           foreach (System.CodeDom.Compiler.CompilerError ce in cr.Errors)    
               Console.WriteLine(ce.ErrorText); 
            
           if (cr.Errors.Count == 0 && cr.CompiledAssembly != null) 
           { 
               Type ObjType = cr.CompiledAssembly.GetType("myclass"); 
               
               try 
               { 
                   if (ObjType != null) 
                   { 
                       myobj = (MyClassBase)Activator.CreateInstance(ObjType); 
                   } 
               } 
               catch (Exception ex) 
               { 
                   Console.WriteLine(ex.Message); 
               } 
               
               return true; 
           } 
           else     
               return false; 
        } 
        
        public double eval(double x, double y) 
        { 
            double val = 0.0; 
            
            if (myobj != null) 
            { 
                val = myobj.eval(x, y);
            } 
            
            return val; 
        } 
    } 
}