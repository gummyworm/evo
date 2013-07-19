/*****************************************************************************/
/* Render.c                                                                  */
/* This is the base component for all things that are to be rendered in the  */
/* engine. Included are some basic functions to add attributes.              */
/*                                                                           */
/*                                                                           */
/* Bryce Wilson                                                              */
/* Created: July 19, 2013                                                    */
/*****************************************************************************/

COMPONENT Renderer {

    /**
     * All functions which inherit Render should override this. 
     * This is where you put drawing code.
     */
    public void Render()
    {

    }
    void Start()
    {

    }
    void Update()
    {

    }
    void Collide(Entity* e)
    {

    }
}
